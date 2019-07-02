#include "handmade.h"

#include "../include/SDL.h"
#include <stdio.h>

#ifdef _WIN32
#include <malloc.h>
#include <windows.h>
#else
#include <dlfcn.h>
#include <fcntl.h>
#include <glob.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
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

global_variable uint32 GlobalWindowWidth = 1920;
global_variable uint32 GlobalWindowHeight = 1080;

#define MAX_CONTROLLERS 4
#define CONTROLLER_AXIS_LEFT_DEADZONE 7849
global_variable SDL_GameController *ControllerHandles[MAX_CONTROLLERS];
global_variable SDL_Haptic *RumbleHandles[MAX_CONTROLLERS];

internal void
CatStrings(size_t SourceACount, char *SourceA,
           size_t SourceBCount, char *SourceB,
           size_t DestCount, char *Dest) {
    // TODO(anthony): Dest bounds checking

    for (int Index = 0;
         Index < SourceACount;
         ++Index) {
        *Dest++ = *SourceA++;
    }

    for (int Index = 0;
         Index < SourceBCount;
         ++Index) {
        *Dest++ = *SourceB++;
    }
    *Dest++ = '\0';
}

internal void
SDLGetEXEFileName(sdl_state *State) {
// NOTE(casey): Never use MAX_PATH in code that is user_facing, because it
// can be dangerous and lead to bad results.
#ifdef _WIN32
    DWORD SizeOfFilename = GetModuleFileNameA(0, State->EXEFileName, sizeof(State->EXEFileName));
    State->OnePastLastEXEFileNameSlash = State->EXEFileName;
    for (char *Scan = State->EXEFileName;
         *Scan;
         ++Scan) {
        if (*Scan == '\\') {
            State->OnePastLastEXEFileNameSlash = Scan + 1;
        }
    }
#else
    memset(State->EXEFileName, 0, sizeof(State->EXEFileName));

    ssize_t SizeOfFilename = readlink("/proc/self/exe", State->EXEFileName, sizeof(State->EXEFileName) - 1);
    State->OnePastLastEXEFileNameSlash = State->EXEFileName;
    for (char *Scan = State->EXEFileName;
         *Scan;
         Scan++) {
        if (*Scan == '/') {
            State->OnePastLastEXEFileNameSlash = Scan + 1;
        }
    }
#endif
}

internal int
StringLength(char *String) {
    int Count = 0;
    while (*String++) {
        ++Count;
    }
    return (Count);
}

internal void
SDLBuildEXEPathFileName(sdl_state *State, char *FileName,
                        int DestCount, char *Dest) {
    CatStrings(State->OnePastLastEXEFileNameSlash - State->EXEFileName, State->EXEFileName,
               StringLength(FileName), FileName,
               DestCount, Dest);
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory) {
    if (Memory) {
#ifdef _WIN32
        VirtualFree(Memory, 0, MEM_RELEASE);
#else
        free(Memory);
#endif
    }
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile) {
    debug_read_file_result Result = {};

#ifdef _WIN32
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(FileHandle, &FileSize)) {
            uint32 FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);
            Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (Result.Contents) {
                DWORD BytesRead;
                if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) &&
                    (FileSize32 == BytesRead)) {
                    // File read successfully
                    Result.ContentsSize = FileSize32;
                } else {
                    // TODO(anthony): Logging
                    DEBUGPlatformFreeFileMemory(Thread, Result.Contents);
                    Result.Contents = 0;
                }
            } else {
                // TODO(anthony): Logging
            }
        } else {
            // TODO(anthony): Logging
        }

        CloseHandle(FileHandle);
    } else {
        // TODO(anthony): Logging
    }
#else
    int FileHandle = open(Filename, O_RDONLY);
    if (FileHandle == -1) {
        return (Result);
    }

    struct stat FileStatus;
    if (fstat(FileHandle, &FileStatus) == -1) {
        close(FileHandle);
        return (Result);
    }
    Result.ContentsSize = SafeTruncateUInt64(FileStatus.st_size);

    Result.Contents = malloc(Result.ContentsSize);
    if (!Result.Contents) {
        close(FileHandle);
        Result.ContentsSize = 0;
        return (Result);
    }

    uint32 BytesToRead = Result.ContentsSize;
    uint8 *NextByteLocation = (uint8 *)Result.Contents;
    while (BytesToRead) {
        ssize_t BytesRead = read(FileHandle, NextByteLocation, BytesToRead);
        if (BytesRead == -1) {
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

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile) {
#ifdef _WIN32
    bool32 Result = false;

    HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE) {
        DWORD BytesWritten;
        if (WriteFile(FileHandle, Memory, MemorySize, &BytesWritten, 0)) {
            // File read successfully
            Result = (BytesWritten == MemorySize);
        } else {
            // TODO(anthony): Logging
        }
        CloseHandle(FileHandle);
    } else {
        // TODO(anthony): Logging
    }
    return (Result);
#else
    int FileHandle = open(Filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (!FileHandle) {
        return (false);
    }

    uint32 BytesToWrite = MemorySize;
    uint8 *NextByteLocation = (uint8 *)Memory;
    while (BytesToWrite) {
        ssize_t BytesWritten = write(FileHandle, NextByteLocation, BytesToWrite);
        if (BytesWritten == -1) {
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
SDLGetLastWriteTime(char *Filename) {
    FILETIME LastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (GetFileAttributesEx((LPCWSTR)Filename, GetFileExInfoStandard, &Data)) {
        LastWriteTime = Data.ftLastWriteTime;
    }
    return (LastWriteTime);
}
#else
inline time_t
SDLGetLastWriteTime(char *Filename) {
    time_t LastWriteTime = 0;

    struct stat FileStatus;
    if (stat(Filename, &FileStatus) == 0) {
        LastWriteTime = FileStatus.st_mtime;
    }
    return (LastWriteTime);
}
#endif

internal sdl_game_code
SDLLoadGameCode(char *SourceDLLName, char *TempDLLName) {
    sdl_game_code Result = {};

    Result.DLLLastWriteTime = SDLGetLastWriteTime(SourceDLLName);

#ifdef _WIN32
    CopyFile((LPCWSTR)SourceDLLName, (LPCWSTR)TempDLLName, FALSE);

    Result.GameCodeDLL = LoadLibraryA((LPCSTR)TempDLLName);

    if (Result.GameCodeDLL) {
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
        int dest = open(TempDLLName, O_WRONLY | O_CREAT, 0644);

        while ((size = read(source, buf, BUFSIZ)) > 0) {
            write(dest, buf, size);
        }
        close(source);
        close(dest);

        Result.GameCodeDLL = dlopen(TempDLLName, RTLD_LAZY);
        if (Result.GameCodeDLL) {
            Result.UpdateAndRender = (game_update_and_render *)dlsym(Result.GameCodeDLL, "GameUpdateAndRender");
            Result.GetSoundSamples = (game_get_sound_samples *)dlsym(Result.GameCodeDLL, "GameGetSoundSamples");
            Result.IsValid = (Result.UpdateAndRender &&
                              Result.GetSoundSamples);
        } else {
            puts(dlerror());
        }
    }
#endif

    if (!Result.IsValid) {
        Result.UpdateAndRender = 0;
        Result.GetSoundSamples = 0;
    }

    return (Result);
}

internal void
SDLUnloadGameCode(sdl_game_code *GameCode) {
    if (GameCode->GameCodeDLL) {
#if _WIN32
        FreeLibrary(GameCode->GameCodeDLL);
#else
        dlclose(GameCode->GameCodeDLL);
#endif

        GameCode->GameCodeDLL = 0;
    }

    GameCode->IsValid = false;
    GameCode->UpdateAndRender = 0;
    GameCode->GetSoundSamples = 0;
}

internal void
SDLOpenGameControllers() {
    int MaxJoysticks = SDL_NumJoysticks();
    int ControllerIndex = 0;

    for (int JoystickIndex = 0;
         JoystickIndex < MaxJoysticks;
         ++JoystickIndex) {
        if (!SDL_IsGameController(JoystickIndex)) {
            continue;
        }
        if (ControllerIndex >= MAX_CONTROLLERS) {
            break;
        }
        ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
        SDL_Joystick *JoystickHandle = SDL_GameControllerGetJoystick(ControllerHandles[ControllerIndex]);
        RumbleHandles[ControllerIndex] = SDL_HapticOpenFromJoystick(JoystickHandle);
        if (SDL_HapticRumbleInit(RumbleHandles[ControllerIndex]) != 0) {
            SDL_HapticClose(RumbleHandles[ControllerIndex]);
            RumbleHandles[ControllerIndex] = 0;
        }
        ControllerIndex++;
    }
}

internal void
SDLCloseGameControllers() {
    for (int ControllerIndex = 0;
         ControllerIndex < MAX_CONTROLLERS;
         ++ControllerIndex) {
        if (ControllerHandles[ControllerIndex]) {
            if (RumbleHandles[ControllerIndex]) {
                SDL_HapticClose(RumbleHandles[ControllerIndex]);
            }
            SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
        }
    }
}

internal void
SDLInitAudio(int32 SamplesPerSecond, int32 BufferSize) {
    SDL_AudioSpec AudioSettings = {};

    AudioSettings.freq = SamplesPerSecond;
    AudioSettings.format = AUDIO_S16LSB;
    AudioSettings.channels = 2;
    AudioSettings.samples = 512;

    SDL_OpenAudio(&AudioSettings, 0);

    printf("Initialized an audio device at frequency %d Hz, %d channels, buffer size %d\n",
           AudioSettings.freq, AudioSettings.channels, AudioSettings.size);

    if (AudioSettings.format != AUDIO_S16LSB) {
        printf("Oops, we didn't get AUDIO_S16LSB as our sample format!");
        SDL_CloseAudio();
    }
}

sdl_window_dimension
SDLGetWindowDimension(SDL_Window *Window) {
    sdl_window_dimension Result;
    SDL_GetWindowSize(Window, &Result.Width, &Result.Height);

    return (Result);
}

internal void
SDLResizeTexture(sdl_offscreen_buffer *Buffer, SDL_Renderer *Renderer, int Width, int Height) {
    if (Buffer->Memory) {
#ifdef _WIN32
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
#else
        munmap(Buffer->Memory, (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel);
#endif
    }

    Buffer->Width = Width;
    Buffer->Height = Height;

    int BytesPerPixel = 4;
    Buffer->BytesPerPixel = BytesPerPixel;

    if (Buffer->Texture) {
        SDL_DestroyTexture(Buffer->Texture);
    }

    Buffer->Texture = SDL_CreateTexture(Renderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        Buffer->Width,
                                        Buffer->Height);

    Buffer->Pitch = Align16(Width * BytesPerPixel);
    int BitmapMemorySize = (Buffer->Width * Buffer->Height) * BytesPerPixel;
#ifdef _WIN32
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
    Buffer->Memory = mmap(0,
                          BitmapMemorySize,
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS,
                          -1, 0);
#endif
}

internal void
SDLDisplayBufferInWindow(sdl_offscreen_buffer *Buffer, SDL_Renderer *Renderer, int WindowWidth, int WindowHeight) {
    SDL_UpdateTexture(Buffer->Texture,
                      0,
                      Buffer->Memory,
                      Buffer->Pitch);

    if ((WindowWidth >= Buffer->Width * 2) &&
        (WindowHeight >= Buffer->Height * 2)) {
        SDL_Rect SrcRect = {0, 0, Buffer->Width, Buffer->Height};
        SDL_Rect DestRect = {0, 0, 2 * Buffer->Width, 2 * Buffer->Height};
        SDL_RenderCopy(Renderer,
                       Buffer->Texture,
                       &SrcRect,
                       &DestRect);
    } else {
#if 0
        int OffsetX = 10;
        int OffsetY = 10;

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_Rect BlackRects[4] = {
            {0, 0, WindowWidth, OffsetY},
            {0, OffsetY + Buffer->Height, WindowWidth, WindowHeight},
            {0, 0, OffsetX, WindowHeight},
            {OffsetX + Buffer->Width, 0, WindowWidth, WindowHeight}};
        SDL_RenderFillRects(Renderer, BlackRects, ArrayCount(BlackRects));
#else
        int OffsetX = 0;
        int OffsetY = 0;
#endif

        SDL_Rect SrcRect = {0, 0, Buffer->Width, Buffer->Height};
        SDL_Rect DestRect = {OffsetX, OffsetY, Buffer->Width, Buffer->Height};
        SDL_RenderCopy(Renderer,
                       Buffer->Texture,
                       &SrcRect,
                       &DestRect);
    }
    SDL_RenderPresent(Renderer);
}

internal void
SDLClearSoundBuffer(sdl_sound_output *SoundOutput) {
    SDL_ClearQueuedAudio(1);
}

internal void
SDLFillSoundBuffer(sdl_sound_output *SoundOutput, int BytesToWrite,
                   game_sound_output_buffer *SoundBuffer) {
    SDL_QueueAudio(1, SoundBuffer->Samples, BytesToWrite);
}

internal void
SDLProcessKeyboardEvent(game_button_state *NewState, bool32 IsDown) {
    if (NewState->EndedDown != IsDown) {
        NewState->EndedDown = IsDown;
        ++NewState->HalfTransitionCount;
    }
}

internal void
SDLProcessGameControllerButton(game_button_state *OldState, bool Value,
                               game_button_state *NewState) {
    NewState->EndedDown = Value;
    NewState->HalfTransitionCount = (OldState->EndedDown != NewState->EndedDown) ? 1 : 0;
}

internal real32
SDLProcessGameControllerAxisValue(int16 Value, int16 DeadZoneThreshold) {
    real32 Result = 0;

    if (Value < -DeadZoneThreshold) {
        Result = (real32)((Value + DeadZoneThreshold) / (32768.0f - DeadZoneThreshold));
    } else if (Value > DeadZoneThreshold) {
        Result = (real32)((Value - DeadZoneThreshold) / (32767.0f - DeadZoneThreshold));
    }

    return (Result);
}

internal void
SDLGetInputFileLocation(sdl_state *State, bool32 InputStream,
                        int SlotIndex, int DestCount, char *Dest) {
    char Temp[64];
    sprintf(Temp, "loop_edit_%d_%s.hmi", SlotIndex, InputStream ? "input" : "state");
    SDLBuildEXEPathFileName(State, Temp, DestCount, Dest);
}

internal sdl_replay_buffer *
SDLGetReplayBuffer(sdl_state *State, unsigned int Index) {
    Assert(Index > 0);
    Assert(Index < ArrayCount(State->ReplayBuffers));
    sdl_replay_buffer *Result = &State->ReplayBuffers[Index];
    return (Result);
}

internal void
SDLBeginRecordingInput(sdl_state *State, int InputRecordingIndex) {
    sdl_replay_buffer *ReplayBuffer = SDLGetReplayBuffer(State, InputRecordingIndex);
    if (ReplayBuffer->MemoryBlock) {
        State->InputRecordingIndex = InputRecordingIndex;

        char FileName[SDL_STATE_FILE_NAME_COUNT];
        SDLGetInputFileLocation(State, true, InputRecordingIndex, sizeof(FileName), FileName);
#ifdef _WIN32
        State->RecordingHandle = CreateFileA(FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

#if 0
        LARGE_INTEGER FilePosition;
        FilePosition.QuadPart = State->TotalSize;
        SetFilePointerEx(State->RecordingHandle, FilePosition, 0, FILE_BEGIN);
#endif

        CopyMemory(ReplayBuffer->MemoryBlock, State->GameMemoryBlock, State->TotalSize);
#else
        State->RecordingHandle = open(FileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

#if 0
        lseek(State->RecordingHandle, State->TotalSize, SEEK_SET);
#endif

        memcpy(ReplayBuffer->MemoryBlock, State->GameMemoryBlock, State->TotalSize);
#endif
    }
}

internal void
SDLEndRecordingInput(sdl_state *State) {
#ifdef _WIN32
    CloseHandle(State->RecordingHandle);
#else
    close(State->RecordingHandle);
#endif
    State->InputRecordingIndex = 0;
}

internal void
SDLBeginInputPlayBack(sdl_state *State, int InputPlayingIndex) {
    sdl_replay_buffer *ReplayBuffer = SDLGetReplayBuffer(State, InputPlayingIndex);
    if (ReplayBuffer->MemoryBlock) {
        State->InputPlayingIndex = InputPlayingIndex;

        char FileName[SDL_STATE_FILE_NAME_COUNT];
        SDLGetInputFileLocation(State, true, InputPlayingIndex, sizeof(FileName), FileName);
#ifdef _WIN32
        State->PlaybackHandle = CreateFileA(FileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);

#if 0
LARGE_INTEGER FilePosition;
FilePosition.QuadPart = State->TotalSize;
SetFilePointerEx(State->PlaybackHandle, FilePosition, 0, FILE_BEGIN);
#endif

        CopyMemory(State->GameMemoryBlock, ReplayBuffer->MemoryBlock, State->TotalSize);
#else
        State->PlaybackHandle = open(FileName, O_RDONLY);

#if 0
lseek(State->PlaybackHandle, State->TotalSize, SEEK_SET);
#endif

        memcpy(State->GameMemoryBlock, ReplayBuffer->MemoryBlock, State->TotalSize);
#endif
    }
}

internal void
SDLEndInputPlayBack(sdl_state *State) {
#ifdef _WIN32
    CloseHandle(State->PlaybackHandle);
#else
    close(State->PlaybackHandle);
#endif
    State->InputPlayingIndex = 0;
}

internal void
SDLRecordInput(sdl_state *State, game_input *NewInput) {
#ifdef _WIN32
    DWORD BytesWritten;
    WriteFile(State->RecordingHandle, NewInput, sizeof(*NewInput), &BytesWritten, 0);
#else
    ssize_t BytesWritten = write(State->RecordingHandle, NewInput, sizeof(*NewInput));
#endif
}

internal void
SDLPlayBackInput(sdl_state *State, game_input *NewInput) {
#ifdef _WIN32
    DWORD BytesRead = 0;
    ReadFile(State->PlaybackHandle, NewInput, sizeof(*NewInput), &BytesRead, 0);
#else
    ssize_t BytesRead = read(State->PlaybackHandle, NewInput, sizeof(*NewInput));
#endif

    if (BytesRead == 0) {
        // NOTE(anthony): We've hit the end of the stream, go back to the beginning
        int PlayingIndex = State->InputPlayingIndex;
        SDLEndInputPlayBack(State);
        SDLBeginInputPlayBack(State, PlayingIndex);
#ifdef _WIN32
        ReadFile(State->PlaybackHandle, NewInput, sizeof(*NewInput), &BytesRead, 0);
#else
        BytesRead = read(State->PlaybackHandle, NewInput, sizeof(*NewInput));
#endif
    }
}

internal void
SDLToggleFullscreen(SDL_Window *Window) {
    uint32 Flags = SDL_GetWindowFlags(Window);
    if (Flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
        SDL_SetWindowFullscreen(Window, 0);
    } else {
        SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}

internal void
SDLProcessPendingEvents(sdl_state *State, game_controller_input *KeyboardController) {
    SDL_Event Event;
    while (SDL_PollEvent(&Event)) {
        switch (Event.type) {
        case SDL_QUIT: {
            GlobalRunning = false;
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            SDL_Keycode KeyCode = Event.key.keysym.sym;
            bool IsDown = (Event.key.state == SDL_PRESSED);

            // NOTE(anthony): SDL has 'repeat", which we'll use to detect repeated keys
            if (Event.key.repeat == 0) {
                if (KeyCode == SDLK_w) {
                    SDLProcessKeyboardEvent(&KeyboardController->MoveUp, IsDown);
                } else if (KeyCode == SDLK_a) {
                    SDLProcessKeyboardEvent(&KeyboardController->MoveLeft, IsDown);
                } else if (KeyCode == SDLK_s) {
                    SDLProcessKeyboardEvent(&KeyboardController->MoveDown, IsDown);
                } else if (KeyCode == SDLK_d) {
                    SDLProcessKeyboardEvent(&KeyboardController->MoveRight, IsDown);
                } else if (KeyCode == SDLK_q) {
                    SDLProcessKeyboardEvent(&KeyboardController->LeftShoulder, IsDown);
                } else if (KeyCode == SDLK_e) {
                    SDLProcessKeyboardEvent(&KeyboardController->RightShoulder, IsDown);
                } else if (KeyCode == SDLK_UP) {
                    SDLProcessKeyboardEvent(&KeyboardController->ActionUp, IsDown);
                } else if (KeyCode == SDLK_LEFT) {
                    SDLProcessKeyboardEvent(&KeyboardController->ActionLeft, IsDown);
                } else if (KeyCode == SDLK_DOWN) {
                    SDLProcessKeyboardEvent(&KeyboardController->ActionDown, IsDown);
                } else if (KeyCode == SDLK_RIGHT) {
                    SDLProcessKeyboardEvent(&KeyboardController->ActionRight, IsDown);
                } else if (KeyCode == SDLK_ESCAPE) {
                    SDLProcessKeyboardEvent(&KeyboardController->Back, IsDown);
                } else if (KeyCode == SDLK_SPACE) {
                    SDLProcessKeyboardEvent(&KeyboardController->Start, IsDown);
                }
#if HANDMADE_INTERNAL
                else if (KeyCode == SDLK_p) {
                    if (IsDown) {
                        GlobalPause = !GlobalPause;
                    }
                } else if (KeyCode == SDLK_l) {
                    if (IsDown) {
                        if (State->InputPlayingIndex == 0) {
                            if (State->InputRecordingIndex == 0) {
                                SDLBeginRecordingInput(State, 1);
                            } else {
                                SDLEndRecordingInput(State);
                                SDLBeginInputPlayBack(State, 1);
                            }
                        } else {
                            SDLEndInputPlayBack(State);
                        }
                    }
                }
#endif
                if (IsDown) {
                    bool AltKeyWasDown = (Event.key.keysym.mod & KMOD_ALT);
                    if (KeyCode == SDLK_F4 && AltKeyWasDown) {
                        GlobalRunning = false;
                    }
                    if (KeyCode == SDLK_RETURN && AltKeyWasDown) {
                        SDL_Window *Window = SDL_GetWindowFromID(Event.window.windowID);
                        if (Window) {
                            SDLToggleFullscreen(Window);
                        }
                    }
                }
            }
        } break;
        case SDL_WINDOWEVENT: {
            switch (Event.window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED: {
                SDL_Window *Window = SDL_GetWindowFromID(Event.window.windowID);
                SDL_Renderer *Renderer = SDL_GetRenderer(Window);
                printf("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d)\n", Event.window.data1, Event.window.data2);
            } break;
            case SDL_WINDOWEVENT_FOCUS_GAINED: {
                printf("SDL_WINDOWEVENT_FOCUS_GAINED\n");
            } break;
            case SDL_WINDOWEVENT_EXPOSED: {
                SDL_Window *Window = SDL_GetWindowFromID(Event.window.windowID);
                SDL_Renderer *Renderer = SDL_GetRenderer(Window);
                sdl_window_dimension Dimension = SDLGetWindowDimension(Window);
                SDLDisplayBufferInWindow(&GlobalBackbuffer, Renderer,
                                         Dimension.Width, Dimension.Height);
            } break;
            }
        } break;
        }
    }
}

inline uint64
SDLGetWallClock(void) {
    uint64 Result = SDL_GetPerformanceCounter();
    return (Result);
}

internal real32
SDLGetSecondsElapsed(uint64 Start, uint64 End) {
    real32 Result = ((real32)(End - Start) /
                     (real32)GlobalPerfCountFrequency);
    return (Result);
}

#if 0
internal void
HandleDebugCycleCounters(game_memory *Memory) {
#if HANDMADE_INTERNAL
    printf("DEBUG CYCLE_COUNTS:\n");
    for (int CounterIndex = 0;
         CounterIndex < ArrayCount(Memory->Counters);
         ++CounterIndex) {
        debug_cycle_counter *Counter = Memory->Counters + CounterIndex;

        if (Counter->HitCount) {
            printf("  %d: %lucy %uh %lucy/h\n",
                   CounterIndex,
                   Counter->CycleCount,
                   Counter->HitCount,
                   Counter->CycleCount / Counter->HitCount);
            Counter->HitCount = 0;
            Counter->CycleCount = 0;
        }
    }
#endif
}

internal void
SDLDebugDrawVertical(sdl_offscreen_buffer *Backbuffer,
                     int X, int Top, int Bottom, uint32 Color) {
    if (Top <= 0) {
        Top = 0;
    }
    if (Bottom > Backbuffer->Height) {
        Bottom = Backbuffer->Height;
    }

    if ((X >= 0) && (X < Backbuffer->Width)) {
        uint8 *Pixel = ((uint8 *)Backbuffer->Memory +
                        X * Backbuffer->BytesPerPixel +
                        Top * Backbuffer->Pitch);
        for (int Y = Top;
             Y < Bottom;
             ++Y) {
            *(uint32 *)Pixel = Color;
            Pixel += Backbuffer->Pitch;
        }
    }
}

inline void
SDLDrawSoundBufferMarker(sdl_offscreen_buffer *Backbuffer,
                         sdl_sound_output *SoundOutput,
                         real32 C, int PadX, int Top, int Bottom,
                         uint32 Value, uint32 Color) {
    real32 XReal32 = (C * (real32)Value);
    int X = PadX + (int)XReal32;
    SDLDebugDrawVertical(Backbuffer, X, Top, Bottom, Color);
}

internal void
SDLDebugSyncDisplay(sdl_offscreen_buffer *Backbuffer,
                    int MarkerCount, sdl_debug_time_marker *Markers,
                    int CurrentMarkerIndex,
                    sdl_sound_output *SoundOutput, real32 TargetSecondsPerFrame) {
    int PadX = 16;
    int PadY = 16;
    int LineHeight = 64;

    real32 C = (real32)(Backbuffer->Width - 2 * PadX) / (real32)SoundOutput->SecondaryBufferSize;
    for (int MarkerIndex = 0;
         MarkerIndex < MarkerCount;
         ++MarkerIndex) {
        sdl_debug_time_marker *ThisMarker = &Markers[MarkerIndex];

        uint32 WriteColor = 0x000000FF;
        uint32 QueuedAudioColor = 0x00FF0000;
        uint32 ExpectedFlipColor = 0x0000FF00;

        int Top = PadY;
        int Bottom = PadY + LineHeight;
        if (MarkerIndex == CurrentMarkerIndex) {
            Top += LineHeight + PadY;
            Bottom += LineHeight + PadY;

            int FirstTop = Top;

            SDLDrawSoundBufferMarker(Backbuffer, SoundOutput, C, MarkerIndex * 30 + PadX,
                                     Top, Bottom, ThisMarker->OutputByteCount, WriteColor);
            Top += LineHeight + PadY;
            Bottom += LineHeight + PadY;
            SDLDrawSoundBufferMarker(Backbuffer, SoundOutput, C, MarkerIndex * 30 + PadX,
                                     Top, Bottom, ThisMarker->QueuedAudioBytes, QueuedAudioColor);
            Top += LineHeight + PadY;
            Bottom += LineHeight + PadY;
            SDLDrawSoundBufferMarker(Backbuffer, SoundOutput, C, MarkerIndex * 30 + PadX,
                                     FirstTop, Bottom, ThisMarker->ExpectedBytesUntilFlip, ExpectedFlipColor);
        }

        SDLDrawSoundBufferMarker(Backbuffer, SoundOutput, C, MarkerIndex * 30 + PadX,
                                 Top, Bottom, ThisMarker->OutputByteCount, WriteColor);

        SDLDrawSoundBufferMarker(Backbuffer, SoundOutput, C, MarkerIndex * 30 + PadX,
                                 Top, Bottom, ThisMarker->QueuedAudioBytes, QueuedAudioColor);

        SDLDrawSoundBufferMarker(Backbuffer, SoundOutput, C, MarkerIndex * 30 + PadX,
                                 Top, Bottom, ThisMarker->ExpectedBytesUntilFlip, ExpectedFlipColor);
    }
}

#endif

int main(int argc, char *argv[]) {
    sdl_state SDLState = {};

    GlobalPerfCountFrequency = SDL_GetPerformanceFrequency();

    SDLGetEXEFileName(&SDLState);

    char SourceGameCodeDLLFullPath[SDL_STATE_FILE_NAME_COUNT];
    char TempGameCodeDLLFullPath[SDL_STATE_FILE_NAME_COUNT];

    SDLBuildEXEPathFileName(&SDLState,
#ifdef _WIN32
                            "handmade.dll",
#else
                            "handmade.so",
#endif
                            sizeof(SourceGameCodeDLLFullPath), SourceGameCodeDLLFullPath);
    SDLBuildEXEPathFileName(&SDLState,
#ifdef _WIN32
                            "handmade_temp.dll",
#else
                            "handmade_temp.so",
#endif
                            sizeof(TempGameCodeDLLFullPath), TempGameCodeDLLFullPath);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_AUDIO);
    SDLOpenGameControllers();

#if HANDMADE_INTERNAL
    DEBUGGlobalShowCursor = true;
#endif

    // Create our window
    SDL_Window *Window = SDL_CreateWindow("Handmade Hero",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          GlobalWindowWidth,
                                          GlobalWindowHeight,
                                          SDL_WINDOW_RESIZABLE);

    if (Window) {
        SDL_ShowCursor(DEBUGGlobalShowCursor ? SDL_ENABLE : SDL_DISABLE);

        SDL_Renderer *Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC);

        if (Renderer) {
            SDLResizeTexture(&GlobalBackbuffer, Renderer, GlobalWindowWidth, GlobalWindowHeight);

            sdl_sound_output SoundOutput = {};

            int MonitorRefreshHz = 60;
            int DisplayIndex = SDL_GetWindowDisplayIndex(Window);
            SDL_DisplayMode Mode = {};
            int DisplayModeResult = SDL_GetDesktopDisplayMode(DisplayIndex, &Mode);
            if (DisplayModeResult == 0 && Mode.refresh_rate > 1) {
                MonitorRefreshHz = Mode.refresh_rate;
            }
            real32 GameUpdateHz = (real32)(MonitorRefreshHz / 2.0f);
            real32 TargetSecondsPerFrame = 1.0f / (real32)GameUpdateHz;

            SoundOutput.SamplesPerSecond = 48000;
            SoundOutput.BytesPerSample = sizeof(int16) * 2;
            SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;

            // TODO(anthony): Actually compute this variance and see what the lowest reasonable value actually is
            SoundOutput.SafetyBytes = (int)(((real32)SoundOutput.SamplesPerSecond * (real32)SoundOutput.BytesPerSample / GameUpdateHz) / 2.0f);
            SDLInitAudio(SoundOutput.SamplesPerSecond, SoundOutput.SecondaryBufferSize);
            SDLClearSoundBuffer(&SoundOutput);
            SDL_PauseAudio(0);

            GlobalRunning = true;
        }
    }
}