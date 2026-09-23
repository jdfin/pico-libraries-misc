#pragma once

#include "hardware/spi.h"

inline uint spix_get_data_bits(const spi_inst_t *spi)
{
    return (spi_get_const_hw(spi)->cr0 & SPI_SSPCR0_DSS_BITS) + 1;
}
