#include "handmade.h"

#include <stdio.h>
#include "../include/SDL.h"

#ifdef _WIN32
#include <windows.h>
#include <malloc.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <x86intrin.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif // MAP_ANONYMOUS
#endif // WIN32

#include "sdl_handmade.h"

global_variable bool32 GlobalRunning;
global_variable bool32 GlobalPause;
global_variable sdl_offscreen_buffer GlobalBackbuffer;
global_variable uint64 GlobalPerfCountFrequency;
global_variable bool32 DEBUGGlobalShowCursor;

#define MAX_CONTROLLERS 4
#define CONTROLLER_AXIS_LEFT_DEADZONE 7849
global_variable SDL_GameController *ControllerHandles[MAX_CONTROLLERS];
global_variable SDL_Haptic *RumbleHandles[MAX_CONTROLLERS];

internal void
CatStrings(size_t SourceACount, char *SourceA,
           size_t SourceBCount, char *SourceB,
           size_t DestCount, char *Dest)
{
    // TODO(anthony): Dest bounds checking

    for (int Index = 0;
         Index < SourceACount;
         ++Index)
    {
        *Dest++ = *SourceA++;
    }

    for (int Index = 0;
         Index < SourceBCount;
         ++Index)
    {
        *Dest++ = *SourceB++;
    }
    *Dest++ = '\0';
}

internal void
SDLGetEXEFileName(sdl_state *State)
{
// NOTE(casey): Never use MAX_PATH in code that is user_facing, because it
// can be dangerous and lead to bad results.
#ifdef _WIN32
    DWORD SizeOfFilename = GetModuleFileNameA(0, State->EXEFileName, sizeof(State->EXEFileName));
    State->OnePastLastEXEFileNameSlash = State->EXEFileName;
    for (char *Scan = State->EXEFileName;
         *Scan;
         ++Scan)
    {
        if (*Scan == '\\')
        {
            State->OnePastLastEXEFileNameSlash = Scan + 1;
        }
    }
#else
    memset(State->EXEFileName, 0, sizeof(State->EXEFileName));

    ssize_t SizeOfFilename = readlink("/proc/self/exe", State->EXEFileName, sizeof(State->EXEFileName) - 1);
    State->OnePastLastEXEFileNameSlash = State->EXEFileName;
    for (char *Scan = State->EXEFileName;
         *Scan;
         Scan++)
    {
        if (*Scan == '/')
        {
            State->OnePastLastEXEFileNameSlash = Scan + 1;
        }
    }
#endif
}

internal int
StringLength(char *String)
{
    int Count = 0;
    while (*String++)
    {
        ++Count;
    }
    return (Count);
}

internal void
SDLBuildEXEPathFileName(sdl_state *State, char *FileName,
                        int DestCount, char *Dest)
{
    CatStrings(State->OnePastLastEXEFileNameSlash - State->EXEFileName, State->EXEFileName,
               StringLength(FileName), FileName,
               DestCount, Dest);
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory)
{
    if (Memory)
    {
#ifdef _WIN32
        VirtualFree(Memory, 0, MEM_RELEASE);
#else
        free(Memory);
#endif
    }
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile)
{
    debug_read_file_result Result = {};

#ifdef _WIN32
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(FileHandle, &FileSize))
        {
            uint32 FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);
            Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (Result.Contents)
            {
                DWORD BytesRead;
                if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) &&
                    (FileSize32 == BytesRead))
                {
                    // File read successfully
                    Result.ContentsSize = FileSize32;
                }
                else
                {
                    // TODO(anthony): Logging
                    DEBUGPlatformFreeFileMemory(Thread, Result.Contents);
                    Result.Contents = 0;
                }
            }
            else
            {
                // TODO(anthony): Logging
            }
        }
        else
        {
            // TODO(anthony): Logging
        }

        CloseHandle(FileHandle);
    }
    else
    {
        // TODO(anthony): Logging
    }
#else
    int FileHandle = open(Filename, O_RDONLY);
    if (FileHandle == -1)
    {
        return (Result);
    }

    struct stat FileStatus;
    if (fstat(FileHandle, &FileStatus) == -1)
    {
        close(FileHandle);
        return (Result);
    }
    Result.ContentsSize = SafeTruncateUInt64(FileStatus.st_size);

    Result.Contents = malloc(Result.ContentsSize);
    if (!Result.Contents)
    {
        close(FileHandle);
        Result.ContentsSize = 0;
        return (Result);
    }

    uint32 BytesToRead = Result.ContentsSize;
    uint8 *NextByteLocation = (uint8 *)Result.Contents;
    while (BytesToRead)
    {
        ssize_t BytesRead = read(FileHandle, NextByteLocation, BytesToRead);
        if (BytesRead == -1)
        {
            free(Result.Contents);
            Result.Contents = 0;
            Result.ContentsSize = 0;
            close(FileHandle);
            return (Result);
        }
        BytesToRead -= BytesRead;
        NextByteLocation += BytesRead;
    }
    close(FileHandle);
#endif

    return (Result);
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile)
{
#ifdef _WIN32
    bool32 Result = false;

    HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten;
        if (WriteFile(FileHandle, Memory, MemorySize, &BytesWritten, 0))
        {
            // File read successfully
            Result = (BytesWritten == MemorySize);
        }
        else
        {
            // TODO(anthony): Logging
        }
        CloseHandle(FileHandle);
    }
    else
    {
        // TODO(anthony): Logging
    }
    return (Result);
#else
    int FileHandle = open(Filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (!FileHandle)
    {
        return (false);
    }

    uint32 BytesToWrite = MemorySize;
    uint8 *NextByteLocation = (uint8 *)Memory;
    while (BytesToWrite)
    {
        ssize_t BytesWritten = write(FileHandle, NextByteLocation, BytesToWrite);
        if (BytesWritten == -1)
        {
            close(FileHandle);
            return (false);
        }
        BytesToWrite -= BytesWritten;
        NextByteLocation += BytesWritten;
    }
    close(FileHandle);

    return (true);
#endif
}

#ifdef _WIN32
inline FILETIME
SDLGetLastWriteTime(char *Filename)
{
    FILETIME LastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (GetFileAttributesEx((LPCWSTR)Filename, GetFileExInfoStandard, &Data))
    {
        LastWriteTime = Data.ftLastWriteTime;
    }
    return (LastWriteTime);
}
#else
inline time_t
SDLGetLastWriteTime(char *Filename)
{
    time_t LastWriteTime = 0;

    struct stat FileStatus;
    if (stat(Filename, &FileStatus) == 0)
    {
        LastWriteTime = FileStatus.st_mtime;
    }
    return (LastWriteTime);
}
#endif

internal sdl_game_code
SDLLoadGameCode(char *SourceDLLName, char *TempDLLName)
{
    sdl_game_code Result = {};

    Result.DLLLastWriteTime = SDLGetLastWriteTime(SourceDLLName);

#ifdef _WIN32
    CopyFile((LPCWSTR)SourceDLLName, (LPCWSTR)TempDLLName, FALSE);

    Result.GameCodeDLL = LoadLibraryA((LPCSTR)TempDLLName);

    if (Result.GameCodeDLL)
    {
        Result.UpdateAndRender = (game_update_and_render *)GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");
        Result.GetSoundSamples = (game_get_sound_samples *)GetProcAddress(Result.GameCodeDLL, "GameGetSoundSamples");

        Result.IsValid = (Result.UpdateAndRender &&
                          Result.GetSoundSamples);
    }
#else
if (Result.DLLLastWriteTime) {
// POSIX-WAY from https://stackoverflow.com/questions/10195343/copy-a-file-in-a-sane-safe-and-efficient-way
char buf[BUFSIZ];
size_t size;

int source = open(SourceDLLName, O_RDONLY, 0);
int dest = open(TempDLLName, O_WRONLY|O_CREAT, 0644);

while ((size = read(source, buf, BUFSIZ)) > 0) {
    write(dest, buf, size);
}
close(source);
close(dest);

Result.GameCodeDLL = dlopen(TempDLLName, RTLD_LAZY);
if (Result.GameCodeDLL) {
    Result.UpdateAndRender = (game_update_and_render*)dlsym(Result.GameCodeDLL, "GameUpdateAndRender");
    Result.GetSoundSamples = (game_get_sound_samples*)dlsym(Result.GameCodeDLL, "GameGetSoundSamples");
    Result.IsValid = (Result.UpdateAndRender &&
    Result.GetSoundSamples);
}
else {
    puts(dlerror());
}
}
#endif

    if (!Result.IsValid)
    {
        Result.UpdateAndRender = 0;
        Result.GetSoundSamples = 0;
    }

    return (Result);
}