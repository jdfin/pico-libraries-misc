#pragma once

#include <cstdint>


// extract: shift value down by 'lsb', then mask according to 'num_bits'
// e.g. if 'abcdefgh' is 8 bits, then extract<uint8_t>('abcdefgh', 3, 2) will return 'de'
template<typename T>
inline T extract(T value, int num_bits, int lsb)
{
    T mask = (1 << num_bits) - 1; // e.g. num_bits=5 -> 0x1f
    return (value >> lsb) & mask;
}


// insert: input/output value 'reg'
//   clear 'num_bits' at position 'lsb'
//   or-in bits in 'val', masked and shifted up to 'lsb'
// e.g. if 'abcdefgh' is 8 bits and 'XYZ' is 3 bits, then
//      insert<uint8_t>('abcdefgh', 'XYZ', 3, 2) results in 'abcXYZgh'
template<typename T>
inline void insert(T& reg, T val, int num_bits, int lsb)
{
    T mask = (1 << num_bits) - 1;
    reg = (reg & ~(mask << lsb)) | ((val & mask) << lsb);
}