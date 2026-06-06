#pragma once

#include <stdint.h>

#include "hardware/pwm.h"
#include "pico/types.h"

// RP2xxx pwm interrupt mux

// The RP2040 pwm has one interrupt to the NVIC, which can be raised by any of
// the 8 slices. The RP2350 pwm has two interrupts to the NVIC, each of which
// can be raised by any of the 12 slices.
//
// The goal of this is to let them handle interrupts a bit more independently.
// In addition to being able to attach a handler to just one of the slices
// (and have a different handler for other slices), this also lets one specify
// a parameter to the handler, one parameter per irq/slice.


#ifdef __cplusplus
extern "C" {
#endif


extern void pwmx_irqn_set_slice_handler(uint irqn, uint slice,
                                        void (*func)(intptr_t), intptr_t arg);


#ifdef __cplusplus
}
#endif
