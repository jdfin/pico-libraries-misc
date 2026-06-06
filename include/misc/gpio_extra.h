#pragma once

#include <stdint.h>

#include "hardware/gpio.h"
#include "pico/types.h"

// RP2xxx gpio interrupt mux

// The goal of this is to attach a different handler to each pin. This also
// lets one specify a parameter to the handler, one parameter per pin. There
// is one table for both cores, under the assumption that a pin will only
// interrupt one of the cores.


#ifdef __cplusplus
extern "C" {
#endif

#define GPIOX_IRQ_EDGE_ANY (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL)

extern void gpiox_set_callback(uint gpio, //
                               void (*callback)(uint, uint32_t, intptr_t),
                               intptr_t arg);


#ifdef __cplusplus
}
#endif
