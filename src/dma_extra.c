#include "dma_extra.h"

#include <assert.h>
#include <stdint.h>
#include <strings.h> // ffs()

#include "hardware/dma.h"
#include "hardware/irq.h"

// This probably needs revisiting if we want different DMA usage on each core

// NUM_DMA_IRQS is 2 for RP2040, and 4 for RP2350
// NUM_DMA_CHANNELS is 16 for both RP2040 and RP2350

static volatile struct {
    void (*func)(intptr_t);
    intptr_t arg;
} dma_irq_mux_handlers[NUM_DMA_IRQS][NUM_DMA_CHANNELS];


// The actual dma irq handlers. Figure out which channel interrupted and
// call the user-installed handler for that channel.
static void dma_irqn_handler(uint irqn)
{
    uint32_t active = dma_hw->irq_ctrl[irqn].ints;

    // datasheet doesn't say what the upper bits are; clear them
    active &= ((1 << NUM_DMA_CHANNELS) - 1); // 16 -> 0xffff

    // For each active interrupt, clear it before calling the handler. That
    // way if the handler is still running when the next interrupt happens,
    // it'll "queue" and the handler will run again.

    while (active != 0) {
        int channel = ffs(active) - 1; // ffs() returns 1..16; channel is 0..15
        dma_irqn_acknowledge_channel(irqn, channel);
        active &= ~(1 << channel);                    // clear bit in active
        dmax_irqn_set_channel_unforce(irqn, channel); // clear the force
        dma_irq_mux_handlers[irqn][channel].func(
            dma_irq_mux_handlers[irqn][channel].arg);
    }
}


static void dma_irq0_handler()
{
    dma_irqn_handler(0);
}


static void dma_irq1_handler()
{
    dma_irqn_handler(1);
}


#if (NUM_DMA_IRQS > 2)
static void dma_irq2_handler()
{
    dma_irqn_handler(2);
}
#endif


#if (NUM_DMA_IRQS > 3)
static void dma_irq3_handler()
{
    dma_irqn_handler(3);
}
#endif


// Clear handler table.
static void dmax_init()
{
    for (uint i = 0; i < NUM_DMA_IRQS; i++) {
        for (uint c = 0; c < NUM_DMA_CHANNELS; c++) {
            dma_irq_mux_handlers[i][c].func = NULL;
            dma_irq_mux_handlers[i][c].arg = 0;
            dma_irqn_set_channel_enabled(i, c, false);
            dma_irqn_acknowledge_channel(i, c);
        }
    }

    // irq_set_exclusive_handler sets the handler for _both_ cores
    // irq_set_enabled enables the interrupt for only the calling core

    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq0_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    irq_set_exclusive_handler(DMA_IRQ_1, dma_irq1_handler);
    irq_set_enabled(DMA_IRQ_1, true);

#if (NUM_DMA_IRQS > 2)
    irq_set_exclusive_handler(DMA_IRQ_2, dma_irq2_handler);
    irq_set_enabled(DMA_IRQ_2, true);
#endif

#if (NUM_DMA_IRQS > 3)
    irq_set_exclusive_handler(DMA_IRQ_3, dma_irq3_handler);
    irq_set_enabled(DMA_IRQ_3, true);
#endif
}


// Install a handler for a channel: void my_handler(intptr_t arg)
void dmax_irqn_set_channel_handler(uint irqn, uint channel,
                                   void (*func)(intptr_t), intptr_t arg)
{
    static int init = 0;

    if (init == 0) {
        dmax_init();
        init = 1;
    }

    assert(irqn < NUM_DMA_IRQS);
    assert(channel < NUM_DMA_CHANNELS);

    dma_irq_mux_handlers[irqn][channel].func = func;
    dma_irq_mux_handlers[irqn][channel].arg = arg;
}
