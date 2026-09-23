#pragma once

#include "pico/stdlib.h"

// For relatively short delays (up to ~35 minutes), it's nice to use signed
// microseconds to handle timer wraparound. One could just use 64-bit
// microseconds, but calls take at least twice as long. For delays that
// doesn't matter, but if we're checking to see if we've reached a point in
// time, it's a couple of inline instructions vs. a call, maybe.

inline bool ge_s32(int32_t t1_us, int32_t t2_us)
{
    return (t1_us - t2_us) >= 0;
}

inline int32_t time_us_s32()
{
    return (int32_t)time_us_32();
}

inline int32_t time_ms_s32()
{
    return (time_us_s32() + 500) / 1000;
}

inline void until_us_s32(int32_t until_us, void (*func)() = nullptr)
{
    while ((time_us_s32() - until_us) < 0) {
        if (func != nullptr)
            (*func)();
        tight_loop_contents();
    }
}

inline void until_ms_s32(int32_t until_ms, void (*func)() = nullptr)
{
    until_us_s32(until_ms * 1000, func);
}

inline void delay_us_s32(int32_t delay_us, void (*func)() = nullptr)
{
    until_us_s32(time_us_s32() + delay_us, func);
}

inline void delay_ms_s32(int32_t delay_ms, void (*func)() = nullptr)
{
    delay_us_s32(delay_ms * 1000, func);
}
