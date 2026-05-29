
#include <stdint.h>
// pico
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
// misc
#include "i2c_extra.h"


// output/open-collector is achieved by setting the output value to zero,
// then toggling between input (hi-z) and output (driven low).
// 0: output (driven low); 1: input (hi-z, pulled up)

static inline void oc_init(uint gpio)
{
    gpio_init(gpio);
    gpio_put(gpio, false);
    // should be input (hi-z) at this point
}

static inline void oc_lo(uint gpio)
{
    gpio_set_dir(gpio, GPIO_OUT);
}

static inline void oc_hi(uint gpio)
{
    gpio_set_dir(gpio, GPIO_IN);
}

// Clear a stuck i2c if a peripheral is holding sda low.
// I2c will need complete reinit after this.
// Return true if i2c is ready to go, false if not.
bool i2cx_bus_clear(uint scl, uint sda)
{
    oc_init(scl);
    gpio_pull_up(scl);
    gpio_set_input_hysteresis_enabled(scl, true);
    gpio_set_slew_rate(scl, GPIO_SLEW_RATE_SLOW);

    oc_init(sda);
    gpio_pull_up(sda);
    gpio_set_input_hysteresis_enabled(sda, true);
    gpio_set_slew_rate(sda, GPIO_SLEW_RATE_SLOW);

    // Both should be hi-z, pulled high at this point.
    // We'll go through and at least do the stop in any case.
    // sda is allowed to change only when scl is low.
    // 100 KHz -> 10 usec clock period.

    // clock scl up to 9 times, or until sda is released
    for (int i = 0; i < 9; i++) {
        if (gpio_get(sda))
            break;
        oc_lo(scl);
        sleep_us(5);
        oc_hi(scl);
        sleep_us(5);
    }

    // i2c stop (sda goes lo->hi while scl is hi)
    oc_lo(sda);
    sleep_us(5);
    oc_hi(sda);
    sleep_us(5);

    gpio_init(scl); // init makes it an input
    gpio_init(sda);

    sleep_us(5);

    // scl and sda should both be pulled high now
    return gpio_get(scl) && gpio_get(sda);
}


// Initiate an i2c write followed by an (optional) i2c read.
// Write wr_len bytes from wr_buf, i2c restart, then read rd_len bytes.
void i2cx_write_read_start(i2c_inst_t *i2c, uint8_t addr, //
                           const uint8_t *wr_buf, int wr_len, int rd_len)
{
    assert(wr_len > 0);

    // Set address
    i2c->hw->enable = 0;
    i2c->hw->tar = addr;
    i2c->hw->enable = 1;

    // First byte automatically causes start and address to go first
    // Last byte might be special.
    while (wr_len-- > 1) {
        while (i2cx_status_tx_fifo_full(i2c))
            ;
        i2c->hw->data_cmd = *wr_buf++;
    }

    // at least one more word to write
    while (i2cx_status_tx_fifo_full(i2c))
        ;

    if (rd_len > 0) {
        // last byte is like the others
        i2c->hw->data_cmd = *wr_buf++;
    } else {
        // no read will follow the write; last byte needs STOP=1
        i2c->hw->data_cmd = I2C_IC_DATA_CMD_STOP_BITS | *wr_buf++;
    }

    // First byte needs RESTART=1, all bytes need CMD=1
    // (This is never used if rd_len == 0)
    uint32_t cmd = I2C_IC_DATA_CMD_RESTART_BITS | I2C_IC_DATA_CMD_CMD_BITS;

    while (rd_len-- > 0) {
        if (rd_len == 0)
            cmd |= I2C_IC_DATA_CMD_STOP_BITS; // last byte goes with STOP=1
        while (i2cx_status_tx_fifo_full(i2c))
            ;
        i2c->hw->data_cmd = cmd;
        cmd &= ~(1 << I2C_IC_DATA_CMD_RESTART_LSB); // only first byte
    }

    // It may take a few microseconds for the transfer to start.
    // Wait here so we can immediately start polling on return if we want.
    uint32_t start_us = time_us_32();
    while (i2cx_running(i2c) == 0 && (time_us_32() - start_us) < 100)
        tight_loop_contents();
}


// Retrieve data read by a previous call to i2cx_write_read_start.
//
// You MUST have already waited for the transfer to complete by polling
// i2cx_running(), so the data is already in the rx fifo.
//
// Return number of bytes found in fifo (0..rd_len). If your rd_buf/rd_len
// here is larger than the rd_len you gave to i2cx_write_read_start, you should
// get only the number of bytes you asked for in that earlier call. The
// two rd_len values can be the same, or you can make this one larger for
// a bit of error checking (to see that the target device acked the right
// number of bytes).
int i2cx_write_read_data(i2c_inst_t *i2c, uint8_t *rd_buf, int rd_len)
{
    int rd_cnt = 0;
    while ((i2c->hw->status & I2C_IC_STATUS_RFNE_BITS) != 0 && rd_len-- > 0) {
        *rd_buf++ = (uint8_t)(i2c->hw->data_cmd);
        rd_cnt++;
    }
    return rd_cnt;
}


// Write control+data words to i2c tx fifo. This is both more and less raw
// than the sdk's i2c_write_byte_raw.
//
// More raw: the passed buffer is 16 bits wide, containing i2c control bits
// in the upper byte and (for writes) data in the lower byte.
//
// Less raw: it always starts a new transfer, and blocks if the tx fifo fills.
//
// It also takes more than one element to write, but I don't know if that is
// more raw or less raw.
//
void i2cx_write_raw(i2c_inst_t *i2c, uint8_t addr, const uint16_t *buf,
                    int buf_len)
{
    i2c->hw->enable = 0;
    i2c->hw->tar = addr;
    i2c->hw->enable = 1;

    // It is intended that buf_len <= fifo size (16 entries) so this never
    // stalls, but if buf_len > fifo size it should still work (by waiting).

    // First word after enable=1 automatically causes start and address to
    // go first. Later words might have the stop or restart bit set.
    while (buf_len-- > 0) {
        while (i2cx_status_tx_fifo_full(i2c))
            ;
        i2c->hw->data_cmd = *buf++;
    }

    // The transfer starts instantly. If a gpio is set after the first byte
    // is put in the fifo the start and gpio appear ~simultaneous on a scope.
}
