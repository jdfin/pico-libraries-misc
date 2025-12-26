#include <stdint.h>
#include <stdio.h>
#include "pretty_io.h"


const char *pretty_hz(uint32_t hz)
{
    static char buf[32]; // for uint32_t ns, max is 10+4+1
    if ((hz % 1000000) == 0)
        sprintf(buf, "%lu MHz", hz / 1000000);
    else if ((hz % 1000) == 0)
        sprintf(buf, "%lu KHz", hz / 1000);
    else
        sprintf(buf, "%lu Hz", hz);
    return buf;
}


const char *pretty_ns(uint32_t ns)
{
    static char buf[32]; // for uint32_t ns, max is 10+5+1
    if ((ns % 1000000000) == 0)
        sprintf(buf, "%lu sec", ns / 1000000000);
    else if ((ns % 1000000) == 0)
        sprintf(buf, "%lu msec", ns / 1000000);
    else if ((ns % 1000) == 0)
        sprintf(buf, "%lu usec", ns / 1000);
    else
        sprintf(buf, "%lu nsec", ns);
    return buf;
}