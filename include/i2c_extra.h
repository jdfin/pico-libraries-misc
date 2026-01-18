#pragma once

#include <stdint.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline int i2c_running(i2c_inst_t *i2c)
{
    return (i2c->hw->status & I2C_IC_STATUS_ACTIVITY_BITS) != 0;
}

extern void i2c_wr_rd_start(i2c_inst_t *i2c, uint8_t addr, //
                            const uint8_t *wr_buf, int wr_len, int rd_len);

extern int i2c_wr_rd_check(i2c_inst_t *i2c, uint8_t *rd_buf, int rd_len);

#ifdef __cplusplus
}
#endif
