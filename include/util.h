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