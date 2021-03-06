#ifndef _SDL_HANDMADE_H
#define _SDL_HANDMADE_H

struct sdl_offscreen_buffer {
    // NOTE(anthony): Pixels are always 32 bits wide, Memory Order BB GG RR XX
    SDL_Texture *Texture;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct sdl_window_dimension {
    int Width;
    int Height;
};

struct sdl_sound_output {
    int SamplesPerSecond;
    uint32 RunningSampleIndex;
    int BytesPerSample;
    uint32 SecondaryBufferSize;
    uint32 SafetyBytes;
};

struct sdl_debug_time_marker {
    uint32 QueuedAudioBytes;
    uint32 OutputByteCount;
    uint32 ExpectedBytesUntilFlip;
};

struct sdl_game_code {

#ifdef _WIN32
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteTime;
#else
    void *GameCodeDLL;
    time_t DLLLastWriteTime;
#endif

    game_update_and_render *UpdateAndRender;
    game_get_sound_samples *GetSoundSamples;

    bool32 IsValid;
};

#define SDL_STATE_FILE_NAME_COUNT 4096
struct sdl_replay_buffer {
#ifdef _WIN32
    HANDLE FileHandle;
#else
    int FileHandle;
#endif
    void *MemoryMap;
    char FileName[SDL_STATE_FILE_NAME_COUNT];
    void *MemoryBlock;
};

struct sdl_state {
    uint64 TotalSize;
    void *GameMemoryBlock;
    sdl_replay_buffer ReplayBuffers[4];

#ifdef _WIN32
    HANDLE RecordingHandle;
    HANDLE PlaybackHandle;
#else
    int RecordingHandle;
    int PlaybackHandle;
#endif
    int InputRecordingIndex;
    int InputPlayingIndex;

    char EXEFileName[SDL_STATE_FILE_NAME_COUNT];
    char *OnePastLastEXEFileNameSlash;
};

#endif