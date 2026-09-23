#pragma once

#include <stdint.h>

#include "hardware/timer.h"


class Timer
{

public:

    Timer(int avg_len, int num_bins = 0) : _avg_len(avg_len), _num_bins(num_bins)
    {
        if (_num_bins > 0)
            _bins = new uint16_t[_num_bins];
        else
            _bins = nullptr;
        reset();
    }

    ~Timer()
    {
        delete[] _bins;
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
        if (_bins != nullptr && !_bins_sat) {
            int idx = (elapsed < (uint32_t)_num_bins) ? (int)elapsed : _num_bins - 1;
            if (_bins[idx] == UINT16_MAX)
                _bins_sat = true;
            else
                _bins[idx]++;
        }
    }

    uint32_t min() { return _min; }
    uint32_t max() { return _max; }
    uint32_t avg() { return (_avg_sum + (_avg_len / 2)) / _avg_len; }

    int num_bins() { return _num_bins; }
    uint16_t bin(int i) { return _bins[i]; }
    bool bins_sat() { return _bins_sat; }

    void reset()
    {
        _min = UINT32_MAX;
        _max = 0;
        _avg_sum = 0;
        _bins_sat = false;
        if (_bins != nullptr) {
            for (int i = 0; i < _num_bins; i++)
                _bins[i] = 0;
        }
    }

private:

    uint32_t _start;
    uint32_t _min;
    uint32_t _max;
    uint32_t _avg_sum;
    int _avg_len;
    int _num_bins;
    uint16_t *_bins;
    bool _bins_sat;

}; // class Timer
