
#include <climits>
#include <cstdint>
// pico
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
// misc
#include "misc/i2c_dev.h"
#include "misc/i2c_extra.h"


I2cDev::I2cDev(i2c_inst_t *i2c, int scl, int sda, int baud) :
    _i2c(i2c),
    _scl(scl),
    _sda(sda),
    _baud_request(baud),
    _baud_actual(0)
    // _dma_ch
    // _dma_cfg
{
    bool i2c_ok = i2cx_bus_clear(_scl, _sda);
    assert(i2c_ok);

    _baud_actual = i2c_init(_i2c, _baud_request);

    gpio_init(_scl);
    gpio_set_function(_scl, GPIO_FUNC_I2C);
    // settings per rp2040 datasheet 4.3.1.3.
    // setting slew rate doesn't make an obvious difference in the waveforms
    gpio_pull_up(_scl);
    gpio_set_input_hysteresis_enabled(_scl, true);
    gpio_set_slew_rate(_scl, GPIO_SLEW_RATE_SLOW);

    gpio_init(_sda);
    gpio_set_function(_sda, GPIO_FUNC_I2C);
    gpio_pull_up(_sda);
    gpio_set_input_hysteresis_enabled(_sda, true);
    gpio_set_slew_rate(_sda, GPIO_SLEW_RATE_SLOW);

    // DMA is only used for writes started with write_start().
    _dma_ch = dma_claim_unused_channel(true);
    _dma_cfg = dma_channel_get_default_config(_dma_ch);
    channel_config_set_dreq(&_dma_cfg, i2c_get_dreq(_i2c, true));
    channel_config_set_transfer_data_size(&_dma_cfg, DMA_SIZE_16);
    channel_config_set_write_increment(&_dma_cfg, false); // i2c DATA_CMD reg
    channel_config_set_read_increment(&_dma_cfg, true);   // source data
}


I2cDev::~I2cDev()
{
    dma_channel_abort(_dma_ch);
    dma_channel_unclaim(_dma_ch);

    i2c_deinit(_i2c);

    gpio_init(_scl);
    gpio_init(_sda);
}


void I2cDev::write_start(uint8_t addr,                               //
                         const uint16_t *buf_sync, int buf_sync_len, //
                         /*const*/ uint16_t *buf_dma, int buf_dma_len)
{
    // This is not expected... assert?
    while (i2cx_running(_i2c))
        ;

    // First send buf_sync. This is expected to all fit in the tx fifo, but if
    // it does not, it'll stall waiting for data to go out.
    i2cx_write_raw(_i2c, addr, buf_sync, buf_sync_len);

    // Now start buf_dma.
    // All except last word should be zero in the upper byte.
    // Last word needs stop bit set.

    // XXX This assumes the MSB of each byte is already zero. Notably, if
    //     buf_dma_len were to change (for the same buf_dma), this would not
    //     hold since another call might have set the stop bit in the wrong
    //     byte. This is also why buf_dma isn't const.

    buf_dma[buf_dma_len - 1] |= I2C_IC_DATA_CMD_STOP_BITS;
    dma_channel_configure(_dma_ch, &_dma_cfg,    //
                          &(_i2c->hw->data_cmd), // destination
                          buf_dma,               // source
                          buf_dma_len,           // xfer count
                          true);                 // go!

    // i2c sends data, and dma reads from memory to the i2c tx fifo for
    // buf_dma_len words. When dma finishes the buffer, the i2c will finish
    // sending whatever is in its tx fifo. The last word in the tx fifo has
    // the STOP_BITS flag set, and it is the sending of the stop bit on the
    // i2c bus that triggers the i2c interrupt.
    //
    // Experimentally, the dma finishes and interrupts about 400 usec before
    // the i2c finishes sending the data. That is roughly correct for a
    // 16 word tx fifo and 1 byte of shift-out data at 400 KHz. (A previous
    // version of this ended with the dma interrupt and polled for i2c done
    // in the dma interrupt handler.)
}


I2cDev::Status I2cDev::write_status()
{
    if ((_i2c->hw->status & I2C_IC_STATUS_ACTIVITY_BITS) != 0) {
        return Status::Busy;
    } else if ((_i2c->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_ABRT_BITS) != 0) {
        // Probably either the address or a data byte wasn't acked. It seems
        // unlikely a program would get to this point if the address wasn't
        // acked. Shut down transfer, reset dma and i2c, return error.
        dma_channel_abort(_dma_ch);
        // Not sure if this is needed. I2C should already be stopped if there
        // was a tx abort, but if the dma snuck one more word into the fifo,
        // this should (?) reset the fifo. If it wasn't reset already.
        _i2c->hw->enable = I2C_IC_ENABLE_ABORT_BITS | I2C_IC_ENABLE_ENABLE_BITS;
        // The bit auto-clears when the abort is done
        while ((_i2c->hw->enable & I2C_IC_ENABLE_ABORT_BITS) != 0)
            tight_loop_contents();
        return Status::Error;
    } else {
        return Status::Ok;
    }
}
