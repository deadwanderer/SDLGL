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