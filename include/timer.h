#pragma once

#include <stdint.h>

#include "hardware/timer.h"


class Timer
{

public:

    Timer(int avg_len) : _avg_len(avg_len)
    {
        reset();
    }

    void start()
    {
        _start = time_us_32();
    }

    void stop()
    {
        uint32_t elapsed = time_us_32() - _start;
        if (elapsed < _min) _min = elapsed;
        if (elapsed > _max) _max = elapsed;
        if (_avg_sum == 0)
            _avg_sum = elapsed * _avg_len;
        else
            _avg_sum = (_avg_sum * (_avg_len - 1) + (_avg_len / 2)) / _avg_len + elapsed;
    }

    uint32_t min() { return _min; }
    uint32_t max() { return _max; }
    uint32_t avg() { return (_avg_sum + (_avg_len / 2)) / _avg_len; }

    void reset()
    {
        _min = UINT32_MAX;
        _max = 0;
        _avg_sum = 0;
    }

private:

    uint32_t _start;
    uint32_t _min;
    uint32_t _max;
    uint32_t _avg_sum;
    int _avg_len;

}; // class Timer
