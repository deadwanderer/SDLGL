/*
    NOTE(anthony):

    HANDMADE_INTERNAL:
        0 - Build for public release
        1 - Build for developer only

    HANDMADE_SLOW:
        0 - No slow code allowed!
        1 - Slow code welcome.
 */

#include <math.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

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

#if HANDMADE_SLOW

#define Assert(Expression) \
    if (!(Expression))     \
    {                      \
        *(int *)0 = 0;     \
    }
#else
#define Assert(Expression)
#endif

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
// TODO(anthony): Swap, min, max ... macros?

inline uint32
SafeTruncateUInt64(uint64 Value)
{
    // TODO(anthony): Defines for maximum values
    Assert(Value <= 0xFFFFFFFF;)
        uint32 Result = (uint32)Value;
    return (Result);
}

struct thread_context
{
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
struct debug_read_file_result
{
    uint32 ContentsSize;
    void *Contents;
};

#endif