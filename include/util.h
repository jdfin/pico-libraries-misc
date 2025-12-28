#pragma once


inline char to_hex(unsigned i)
{
    if (i <= 9)
        return '0' + i;
    else if (i <= 15)
        return 'a' + (i - 10);
    else
        return '?';
}


inline bool is_xip(const void *addr)
{
    return (addr >= (void *)XIP_BASE) && (addr < (void *)SRAM_BASE);
}


inline bool is_ram(const void *addr)
{
    return (addr >= (void *)SRAM_BASE) && (addr < (void *)SRAM_END);
}

