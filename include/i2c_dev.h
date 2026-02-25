#pragma once

#include <cstdint>
// pico
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
// misc
#include "i2c_extra.h"


class I2cDev
{

public:

    I2cDev(i2c_inst_t *dev, int scl, int sda, int baud) :
        _dev(dev),
        _scl(scl),
        _sda(sda),
        _baud_req(baud),
        _rd_buf(nullptr),
        _rd_len(0)
    {
        _baud_act = i2c_init(_dev, _baud_req);

        gpio_set_function(_scl, GPIO_FUNC_I2C);
        gpio_pull_up(_scl);

        gpio_set_function(_sda, GPIO_FUNC_I2C);
        gpio_pull_up(_sda);
    }

    ~I2cDev() = default;

    uint baud() const
    {
        return _baud_act;
    }

    void reset()
    {
        _baud_act = i2c_init(_dev, _baud_req);
    }

    int read_sync(uint8_t addr, uint8_t *buf, int buf_len, //
                  bool nostop = false, uint timeout_us = 100'000)
    {
        return i2c_read_timeout_us(_dev, addr, buf, buf_len, //
                                   nostop, timeout_us);
    }

    int write_sync(uint8_t addr, const uint8_t *buf, int buf_len, //
                   bool nostop = false, uint timeout_us = 100'000)
    {
        return i2c_write_timeout_us(_dev, addr, buf, buf_len, //
                                    nostop, timeout_us);
    }

    // rd_buf is supplied here to support dma someday.
    // At the moment it could be supplied to write_read_async_check instead.
    int write_read_async_start(uint8_t addr,                      //
                               const uint8_t *wr_buf, int wr_len, //
                               uint8_t *rd_buf = nullptr, int rd_len = 0)
    {
        _rd_buf = rd_buf;
        _rd_len = rd_len;
        i2c_write_read_start(_dev, addr, wr_buf, wr_len, rd_len);
        return 0;
    }

    bool busy() const
    {
        return i2c_running(_dev);
    }

    int write_read_async_check()
    {
        return i2c_write_read_check(_dev, _rd_buf, _rd_len);
    }

private:

    i2c_inst_t *const _dev;
    const int _scl;
    const int _sda;
    const uint _baud_req; // requested baud

    uint _baud_act; // actual baud

    // for async read
    uint8_t *_rd_buf;
    int _rd_len;

}; // class I2cDev
