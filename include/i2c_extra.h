#pragma once

#include <stdint.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool i2cx_bus_clear(uint scl, uint sda);

static inline int i2cx_running(i2c_inst_t *i2c)
{
    return (i2c->hw->status & I2C_IC_STATUS_ACTIVITY_BITS) != 0;
}

static inline int i2cx_status_tx_fifo_full(i2c_inst_t *i2c)
{
    return (i2c->hw->status & I2C_IC_STATUS_TFNF_BITS) == 0;
}

extern void i2cx_write_read_start(i2c_inst_t *i2c, uint8_t addr,     //
                                  const uint8_t *wr_buf, int wr_len, //
                                  int rd_len);

extern int i2cx_write_read_data(i2c_inst_t *i2c, //
                                uint8_t *rd_buf, int rd_len);

extern void i2cx_write_raw(i2c_inst_t *i2c, uint8_t addr, //
                           const uint16_t *buf, int buf_len);

#ifdef __cplusplus
}
#endif
