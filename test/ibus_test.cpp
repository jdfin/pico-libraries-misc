
#include <cassert>
#include <cstdint>
#include <cstdio>
//
#include "pico/stdio.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"
#include "pico/time.h"
//
#include "hardware/uart.h"
//
#include "misc/stdio_extra.h"
//
#include "misc/ibus.h"

static constexpr int uart_rx_gpio = 1;        // bike v0: 21, v1: 1
static constexpr int uart_baud = 115'200;     //
static uart_inst_t * const uart_inst = uart0; // bike v0: uart1, v1: uart0

//////////////////////////////////////////////////////////////////////////////

int main()
{
    stdiox_wait_usb(-1);

    sleep_ms(100);

    printf("\n");
    printf("ibus_test\n");
    printf("\n");

    gpio_set_function(uart_rx_gpio, UART_FUNCSEL_NUM(uart_inst, uart_rx_gpio));

    uart_init(uart_inst, uart_baud);

    IBus ibus;

    while (true) {

        while (!uart_is_readable(uart_inst))
            tight_loop_contents();

        uint8_t byte;
        uart_read_blocking(uart_inst, &byte, 1);

        if (ibus.data(byte)) {
            for (int i = 1; i <= ibus.channels; i++)
                printf("%6u", ibus.channel(i));
            printf("\n");
        }

    } // while (true)

    return 0;
}
