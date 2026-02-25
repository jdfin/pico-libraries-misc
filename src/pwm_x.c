#include <stdint.h>
#include <strings.h> // ffs()
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "pwm_x.h"


// handlers for 8 slices
static volatile struct {
    void (*func)(void*);
    void *arg;
} pwm_irq_mux_handlers[NUM_PWM_SLICES];


// The actual pwm interrupt handler. Figure out which slice interrupted and
// call the user-installed handler for that slice.
static void pwm_irq_handler()
{
    uint32_t active = pwm_get_irq_status_mask();

    // datasheet doesn't say what the upper bits are; clear them
    active &= ((1 << NUM_PWM_SLICES) - 1); // 8 -> 0xff

    // For each active interrupt, clear it before calling the handler. That
    // way if the handler is still running when the next interrupt happens,
    // it'll "queue" and the handler will run again. This effect is observed
    // when sending DCC bits, and the processing occasionally takes slightly
    // longer than a bit time.

    while (active != 0) {
        int slice = ffs(active) - 1; // ffs() returns 1..8; slice is 0..7
        pwm_clear_irq(slice); // uint slice
        active &= ~(1 << slice); // clear bit in active
        pwm_irq_mux_handlers[slice].func(pwm_irq_mux_handlers[slice].arg);
    }
}


// Install a handler for a slice: void my_handler(void *arg)
void pwm_irq_mux_connect(uint slice, void (*func)(void*), void *arg)
{
    static int init = 0;

    if (init == 0) {
        for (uint slice = 0; slice < NUM_PWM_SLICES; slice++) {
            pwm_irq_mux_handlers[slice].func = NULL;
            pwm_irq_mux_handlers[slice].arg = NULL;
            pwm_set_irq_enabled(slice, false);
            pwm_clear_irq(slice);
        }
        irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irq_handler);
        irq_set_enabled(PWM_IRQ_WRAP, true);
        init = 1;
    }

    pwm_irq_mux_handlers[slice].func = func;
    pwm_irq_mux_handlers[slice].arg = arg;
}
