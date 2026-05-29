
#include <cassert>
#include <cstdint>
#include <cstdio>
// pico
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h"
// misc
#include "i2c_dev.h"

static i2c_inst_t *i2c_dev = i2c1;
static constexpr int sda_gpio = 14;
static constexpr int scl_gpio = 15;
static constexpr int i2c_baud = 100'000;


int main()
{
    stdio_init_all();

    while (!stdio_usb_connected())
        tight_loop_contents();

    sleep_ms(10);

    printf("\n");
    printf("i2c_dev_test\n");
    printf("\n");

    I2cDev i2c(i2c_dev, scl_gpio, sda_gpio, i2c_baud);

    printf("i2c_dev_test: i2c running at %u Hz\n", i2c.baud());

    while (true) {
        printf("\n");
        printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

        for (int addr = 0; addr < 128; addr++) {

            if (addr % 16 == 0)
                printf("%02x ", addr);

            uint8_t data;
            int ret = i2c.read_timeout_us(addr, &data, 1);

            printf(ret < 0 ? "." : "@");
            printf(addr % 16 == 15 ? "\n" : "  ");
        }

        sleep_ms(2000);
    }

    return 0;
}
