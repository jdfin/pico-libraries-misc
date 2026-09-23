
#include <stdint.h>
//
#include "pico/status_led.h"
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h"
//
#include "misc/stdio_extra.h"


static const int32_t led_on_us = 100000;  // 0.1s
static const int32_t led_off_us = 900000; // 0.9s


bool stdiox_wait_usb(int32_t timeout_us)
{
    status_led_init(); // turns it off if it was on

    stdio_init_all();

    int32_t now_us = (int32_t)time_us_32();
    int32_t led_time_us = now_us;
    int32_t end_us = now_us + timeout_us;

    // timeout_us == 0 will never execute the while loop
    // timeout_us == -1 means "wait forever"
    while (timeout_us == -1 || (end_us - (int32_t)time_us_32()) > 0) {
        if (stdio_usb_connected()) {
            status_led_set_state(false);
            sleep_ms(100);
            return true;
        }
        // time to toggle the led?
        if (((int32_t)time_us_32() - led_time_us) > 0) {
            if (status_led_get_state()) {
                status_led_set_state(false);
                led_time_us += led_off_us;
            } else {
                status_led_set_state(true);
                led_time_us += led_on_us;
            }
        }
        tight_loop_contents();
    }
    status_led_set_state(false);
    return false;
}
