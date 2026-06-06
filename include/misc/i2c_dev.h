#pragma once

#include <cassert>
#include <cstdint>
// pico
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
// misc
#include "misc/i2c_extra.h"


// This adds some asynchronous capabilities over using just the pico sdk's
// i2c_* functions.
//
// Motivation is that even for short transactions, each byte takes at least
// nine clocks (data + ack), or 22.5 usec, so for example, writing a one-
// byte register might take [address + register + data] at least 67.5 usec,
// or a 4-byte register takes [address + register + 4 * data] at least 135
// usec, which can be a bit long to spin.
//
// There are required support functions in i2c_extra (which contains things
// more-or-less at the pico sdk api level).
//
// The general model is that one starts a transaction with one call, which
// typically takes a few microseconds, then periodically checks back for
// completion and results.
//
// Short async writes or reads - these are transfers that fit entirely in the
// i2c fifo (16 bytes). This has been used to poll a touchscreen, for example.
// For a short write (e.g. a device register), use:
//   write_read_start() - puts data to tx and commands to rx in fifo
//   write_read_busy() - indicates busy or done
//   write_read_data() - to retrieve data if any was read
// Again, each call here takes a few microseconds, while the overall transfer
// could be hundreds of usec.
//
// Long async writes - arbitrary length writes, but no reading. This uses dma
// to continue filling the tx fifo. The main gotcha here is that the tx buffer
// has to be 16 bits per byte sent, and the upper byte has to be zero. This
// has been used to update a small OLED screen; although the framebuffer would
// normally be 1024 uint8_t, this requires it to be 1024 uint16_t.
//   write_start() - takes a (typically) small buffer that will be written
//                   immediately (should fit in the fifo), plus a longer
//                   buffer that will use dma
//   write_status() - returns busy, done, or error.
// For the OLED use, starting is < 10 usec, status check less than that, and
// the overall transfer is about 25 msec.
//
// Early versions of this used interrupts to detect the completion of dma or
// end of i2c activity, but they don't really add any value since the model is
// we poll for completion anyway.


class I2cDev
{

public:

    I2cDev(i2c_inst_t *dev, int scl, int sda, int baud);

    ~I2cDev();

    uint baud() const
    {
        return _baud_actual;
    }

    void reset()
    {
        _baud_actual = i2c_init(_i2c, _baud_request);
    }

    // Synchronous read.
    int read_timeout_us(uint8_t addr, uint8_t *buf, int buf_len,
                        bool nostop = false, uint timeout_us = 100'000)
    {
        return i2c_read_timeout_us(_i2c, addr, buf, buf_len, //
                                   nostop, timeout_us);
    }

    // Synchronous write.
    int write_timeout_us(uint8_t addr, const uint8_t *buf, int buf_len,
                         bool nostop = false, uint timeout_us = 100'000)
    {
        return i2c_write_timeout_us(_i2c, addr, buf, buf_len, //
                                    nostop, timeout_us);
    }

    // Start write with dma.
    void write_start(uint8_t addr,                               //
                     const uint16_t *buf_sync, int buf_sync_len, //
                     /*const*/ uint16_t *buf_dma, int buf_dma_len);

    enum class Status {
        Ok,
        Busy,
        Error,
    };

    // Check write with dma status.
    Status write_status();

    // Start a (typically) short write and read.
    // It is expected that wr_len + rd_len <= fifo length.
    // If the fifo fills up, this will block waiting for room.
    void write_read_start(uint8_t addr,                      //
                          const uint8_t *wr_buf, int wr_len, //
                          int rd_len = 0)
    {
        i2cx_write_read_start(_i2c, addr, wr_buf, wr_len, rd_len);
    }

    // Check to see if write/read is done.
    bool write_read_busy() const
    {
        return i2cx_running(_i2c);
    }

    // Retrieve data from previous write/read.
    // (Don't need to call this if it was write only.)
    int write_read_data(uint8_t *buf, int buf_len)
    {
        return i2cx_write_read_data(_i2c, buf, buf_len);
    }

private:

    i2c_inst_t *const _i2c;
    const int _scl;
    const int _sda;
    const uint _baud_request;

    uint _baud_actual;

    // for dma writes
    uint _dma_ch;
    dma_channel_config _dma_cfg;

}; // class I2cDev
