#ifndef _HANDMADE_PLATFORM_H
#define _HANDMADE_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include <math.h>
#include <stdint.h>
    
    typedef int8_t int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;
    typedef int32 bool32;
    
    typedef uint8_t uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;
    
    typedef float real32;
    typedef double real64;
    
    struct thread_context {
        int Placeholder;
    };
    
    /*
        NOTE(anthony): Services that the platform layer provides to the game.
     */
    
#if HANDMADE_INTERNAL
    /* IMPORTANT(anthony):
        These are NOT for doing anything in the shipping game --
        They are blocking, and the write doesn't protect against lost data!
     */
    typedef struct debug_read_file_result {
        uint32 ContentsSize;
        void *Contents;
    } debug_read_file_result;
    
#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context *Thread, void *Memory)
    typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);
    
#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context *Thread, char *Filename)
    typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);
    
#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context *Thread, char *Filename, uint32 MemorySize, void *Memory)
    typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);
    
    enum {
        /* 0 */ DebugCycleCounter_GameUpdateAndRender,
        /* 1 */ DebugCycleCounter_RenderGroupToOutput,
        /* 2 */ DebugCycleCounter_DrawRectangleSlowly,
        /* 3 */ DebugCycleCounter_ProcessPixel,
        /* 4 */ DebugCycleCounter_DrawRectangleQuickly,
        DebugCycleCounter_Count,
    };
    
    typedef struct debug_cycle_counter {
        uint64 CycleCount;
        uint32 HitCount;
    } debug_cycle_counter;
    
#endif
    
    /*
        NOTE(anthony): Services that the game provides to the platform layer.
        (this may expand in the future -- sound on separate thread, etc.)
     */
    
    // FOUR THINGS - timing, controller/keyboard input, bitmap buffer to use, sound buffer to use
    
    // TODO(anthony): In the future, rendering _specifically_ will become a three-tiered abstraction
    struct game_offscreen_buffer {
        // NOTE(anthony): Pixels are always 32 bits wide, memory order BB GG RR XX
        void *Memory;
        int Width;
        int Height;
        int Pitch;
        int BytesPerPixel;
    };
    
    struct game_sound_output_buffer {
        int SamplesPerSecond;
        int SampleCount;
        int16 *Samples;
    };
    
    struct game_button_state {
        int HalfTransitionCount;
        bool32 EndedDown;
    };
    
    struct game_controller_input {
        bool32 IsConnected;
        bool32 IsAnalog;
        real32 StickAverageX;
        real32 StickAverageY;
        
        union {
            game_button_state Buttons[12];
            struct
            {
                game_button_state MoveUp;
                game_button_state MoveDown;
                game_button_state MoveLeft;
                game_button_state MoveRight;
                
                game_button_state ActionUp;
                game_button_state ActionDown;
                game_button_state ActionLeft;
                game_button_state ActionRight;
                
                game_button_state LeftShoulder;
                game_button_state RightShoulder;
                
                game_button_state Back;
                game_button_state Start;
                
                // NOTE(anthony): All buttons must be added above this line
                
                game_button_state Terminator;
            };
        };
    };
    
    struct game_input {
        game_button_state MouseButtons[5];
        int32 MouseX, MouseY, MouseZ;
        
        real32 dtForFrame;
        
        // TODO(anthony): Insert clock values here.
        game_controller_input Controllers[5];
    };
    
    struct game_memory {
        bool32 IsInitialized;
        
        uint64 PermanentStorageSize;
        void *PermanentStorage; // NOTE(anthony): Required to be cleared to zero at startup.
        
        uint64 TransientStorageSize;
        void *TransientStorage; // NOTE(anthony): Required to be cleared to zero at startup.
        
#if HANDMADE_INTERNAL
        debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
        debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
        debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
        
        debug_cycle_counter Counters[DebugCycleCounter_Count];
#endif
    };
    
#define GAME_UPDATE_AND_RENDER(name) void name(thread_context *Thread, game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
    typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
    
#define GAME_GET_SOUND_SAMPLES(name) void name(thread_context *Thread, game_memory *Memory, game_sound_output_buffer *SoundBuffer)
    typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);
    
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HANDMADE_INTERNAL