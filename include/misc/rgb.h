#pragma once

#include "hardware/gpio.h"


class Rgb
{

public:

    Rgb(int red_gpio, int grn_gpio, int blu_gpio) :
        _red_gpio(red_gpio),
        _grn_gpio(grn_gpio),
        _blu_gpio(blu_gpio),
        _set(false)
    {
        // all outputs, low
        gpio_init(_red_gpio);
        gpio_put(_red_gpio, false);
        gpio_set_dir(_red_gpio, GPIO_OUT);

        gpio_init(_grn_gpio);
        gpio_put(_grn_gpio, false);
        gpio_set_dir(_grn_gpio, GPIO_OUT);

        gpio_init(_blu_gpio);
        gpio_put(_blu_gpio, false);
        gpio_set_dir(_blu_gpio, GPIO_OUT);
    }

    ~Rgb()
    {
        // all inputs
        gpio_init(_red_gpio);
        _red_gpio = -1;
        gpio_init(_grn_gpio);
        _grn_gpio = -1;
        gpio_init(_blu_gpio);
        _blu_gpio = -1;
    }

    // clang-format off
    inline void off(int delay_ms=0)    { set(false, false, false, delay_ms); }
    inline void red(int delay_ms=0)    { set(true,  false, false, delay_ms); }
    inline void green(int delay_ms=0)  { set(false, true,  false, delay_ms); }
    inline void blue(int delay_ms=0)   { set(false, false, true,  delay_ms); }
    inline void yellow(int delay_ms=0) { set(true,  true,  false, delay_ms); }
    inline void purple(int delay_ms=0) { set(true,  false, true,  delay_ms); }
    inline void cyan(int delay_ms=0)   { set(false, true,  true,  delay_ms); }
    inline void white(int delay_ms=0)  { set(true,  true,  true,  delay_ms); }
    // clang-format on

    void loop()
    {
        if (_set && (int32_t(time_us_32()) - _set_us) >= 0)
            set(_set_red, _set_grn, _set_blu);
    }

private:

    int _red_gpio;
    int _grn_gpio;
    int _blu_gpio;

    // for a future change, what time and what settings
    bool _set;
    int32_t _set_us;
    bool _set_red;
    bool _set_grn;
    bool _set_blu;

    inline void set(bool red, bool grn, bool blu, int delay_ms=0)
    {
        if (delay_ms == 0) {
            gpio_put(_red_gpio, red);
            gpio_put(_grn_gpio, grn);
            gpio_put(_blu_gpio, blu);
            _set = false;
        } else {
            _set_red = red;
            _set_grn = grn;
            _set_blu = blu;
            _set_us = int32_t(time_us_32()) + 1000 * delay_ms;
            _set = true;
        }
    }

}; // class Rgb
