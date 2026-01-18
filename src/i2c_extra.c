
#include <stdint.h>
// pico
#include "hardware/i2c.h"
#include "pico/stdlib.h"
// misc
#include "i2c_extra.h"


// Functions here support non-blocking i2c operations, with the significant
// limitations:
//
// 1. We're always doing a just a write, or a write/restart/read.
//
// 2. Everything fits in the hardware fifos (16 slots), so you can (for
//    example) write or read a register, or read a few consecutive registers.
//    Larger transfers would need DMA support.
//
// 3. There's no error checking here. That's gotta be added.
//
// If these are okay, and it generally does for stuff like reading or writing
// device registers, then we can start a transfer, return to the caller, and
// later check for completion and retrieve results.
//
// The motivation is to avoid blocking while waiting for i2c transfers to
// complete. Starting a write/read takes microseconds, checking for done
// takes near zero time, and retrieving the results takes a few more
// microseconds. The actual transfer itself on the i2c bus is likely a
// couple hundred microseconds, so we're avoiding that spin time.
//
// Writing a register
//
// Start a write by calling i2c_wr_rd_start(). It will often be a short write
// of a byte or two to select a register, followed by a value to write
// (usually one to four bytes, but it could be values for several consecutive
// registers), and a read count of zero. i2c_wr_rd_start returns immediately
// after writing some write commands to the transmit fifo. Since the read
// count is zero, the last write command includes a STOP condition.
//
// Next, periodically check for done by calling i2c_running(). When it returns
// false, the transfer is done and there's nothing else to do.
//
// Reading a register
//
// Start a read by calling i2c_wr_rd_start(). It will be a short write of a
// byte or two to select a register, followed by a read count of however many
// bytes you want to read (often one to four, but could be more).
// i2c_wr_rd_start returns immediately after writing some write and read
// commands to the transmit fifo. Since the read count is nonzero, the first
// read command will have the RESTART flag set. The last read command will
// have the STOP flag set (possibly the same one).
//
// Next, periodically check for done by calling i2c_running(). When it returns
// false, the transfers (write, restart, read) are done, and data received is
// in the receive fifo. Call i2c_wr_rd_check() to retrieve the received data.
// It will read as much as it can from the fifo, until the fifo is empty or
// the requested read length is satisfied, then return the number of bytes
// read.

// Initiate an i2c write followed by an (optional) i2c read.
// Write wr_len bytes from wr_buf, i2c restart, then read rd_len bytes.
void i2c_wr_rd_start(i2c_inst_t *i2c, uint8_t addr, //
                    const uint8_t *wr_buf, int wr_len, int rd_len)
{
    // Set address
    i2c->hw->enable = 0;
    i2c->hw->tar = addr;
    i2c->hw->enable = 1;

    // First byte automatically causes start and address to go first
    while (wr_len-- > 1) // last byte might be special
        i2c->hw->data_cmd = *wr_buf++;
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
        i2c->hw->data_cmd = cmd;
        cmd &= ~(1 << I2C_IC_DATA_CMD_RESTART_LSB); // only first byte
    }

    // It takes a few microseconds for the transfer to start.
    // Wait here so we can immediately start polling on return if we want.
    uint32_t start_us = time_us_32();
    while (i2c_running(i2c) == 0 && (time_us_32() - start_us) < 100)
        tight_loop_contents();
}


// Retrieve data read by a previous call to i2c_wr_rd_start.
//
// You MUST have already waited for the transfer to complete by polling
// i2c_running(), so the data is already in the rx fifo.
//
// Return number of bytes found in fifo (0..rd_len). If your rd_buf/rd_len
// here is larger than the rd_len you gave to i2c_wr_rd_start, you should
// get only the number of bytes you asked for in that earlier call. The
// two rd_len values can be the same, or you can make this one larger for
// a bit of error checking (see that the target device acked the right
// number of bytes).
int i2c_wr_rd_check(i2c_inst_t *i2c, uint8_t *rd_buf, int rd_len)
{
    int rd_cnt = 0;
    while ((i2c->hw->status & I2C_IC_STATUS_RFNE_BITS) != 0 && rd_len-- > 0) {
        *rd_buf++ = (uint8_t)(i2c->hw->data_cmd);
        rd_cnt++;
    }
    return rd_cnt;
}
