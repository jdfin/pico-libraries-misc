#pragma once

#include <initializer_list>

#include "hardware/gpio.h"


class DbgGpio
{

public:

    DbgGpio(int gpio) : _gpio(gpio)
    {
        if (_gpio >= 0) {
            gpio_put(_gpio, 1);
        }
    }

    ~DbgGpio()
    {
        if (_gpio >= 0) {
            gpio_put(_gpio, 0);
        }
    }

    static void init(int gpio)
    {
        if (gpio >= 0) {
            gpio_put(gpio, 0);
            gpio_init(gpio);
            gpio_set_dir(gpio, GPIO_OUT);
        }
    }

    static void init(std::initializer_list<int> gpios)
    {
        for (int gpio : gpios) {
            if (gpio >= 0) {
                init(gpio);
            }
        }
    }

private:

    int _gpio;

}; // class DbgGpio
