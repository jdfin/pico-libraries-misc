#pragma once

#include <cstdint>
//
#include "pico/stdlib.h"


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


// convert an XIP address to an XIP address that won't update the cache
//
// XIP_BASE/XIP_NOCACHE_NOALLOC_BASE are used (rather than a hardcoded
// address-bit-pattern trick) because the offset between the aliases is not
// the same on every platform: on RP2040 XIP_NOCACHE_NOALLOC_BASE is
// XIP_BASE + 0x03000000, but on RP2350 it's XIP_BASE + 0x04000000, so a
// formula tuned for one silently computes a bogus address (landing in an
// unrelated/unmapped region) on the other.
inline const void *xip_nocache(const void *xip_adrs)
{
    if (is_xip(xip_adrs)) {
        uintptr_t offset = reinterpret_cast<uintptr_t>(xip_adrs) - XIP_BASE;
        xip_adrs = reinterpret_cast<const void *>(XIP_NOCACHE_NOALLOC_BASE + offset);
    }
    return xip_adrs;
}


inline const char *mem_name(const void *p)
{
    if (is_ram(p))
        return "RAM";
    else if (is_xip(p))
        return "XIP";
    else
        return "???";
}
