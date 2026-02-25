#pragma once

#include "hardware/pwm.h"

// RP2040 pwm interrupt mux

// The RP2040 pwm has one interrupt to the NVIC, which can be raised by any of
// the 8 slices. The goal of this is to let them handle interrupts a bit more
// independently. In addition to being able to attach a handler to just one of
// the slices (and have a different handler for other slices), this also lets
// one specify a parameter to the handler, one parameter per slice.


#ifdef __cplusplus
extern "C" {
#endif


// install a handler
extern void pwm_irq_mux_connect(uint slice_num, void (*func)(void*), void *arg);


#ifdef __cplusplus
}
#endif