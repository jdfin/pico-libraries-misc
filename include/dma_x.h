#pragma once

#include <stdint.h>

#include "hardware/dma.h"

// RP2040 dma interrupt mux

// The RP2040 dma has two interrupts to the NVIC (irq0 and irq1), either of
// which can be raised by any of the 16 channels. The goal of this is to let
// them handle interrupts a bit more independently. In addition to being able
// to attach a handler to just one of the channels (and have a different
// handler for other channels), this also lets one specify a parameter to the
// handler, one parameter per channel.


#ifdef __cplusplus
extern "C" {
#endif


// install a handler
extern void dmax_irqn_set_channel_handler(uint irqn, uint channel_num,
                                          void (*func)(intptr_t), intptr_t arg);


// enable/disable interrupts for a channel
static inline void dmax_irqn_set_channel_enabled(uint irqn, uint channel,
                                                 bool enable)
{
    dma_irqn_set_channel_enabled(irqn, channel, enable);
}


// trigger an interrupt for a channel
static inline void dmax_irqn_set_channel_force(uint irqn, uint channel)
{
    dma_hw->irq_ctrl[irqn].intf |= (1u << channel);
}


// untrigger an interrupt for a channel
static inline void dmax_irqn_set_channel_unforce(uint irqn, uint channel)
{
    dma_hw->irq_ctrl[irqn].intf &= ~(1u << channel);
}


#ifdef __cplusplus
}
#endif
