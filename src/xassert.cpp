#include <cstdio>
#include "sys_led.h"
#include "xassert.h"


void xassert_func(const char *file, int line, const char *expr)
{
    SysLed::init();             // ok if this has already been called
    SysLed::pattern(250, 250);  // blink at 2 Hz

    // assumes stdio_init_all has been called
    printf("assertion \"%s\" failed: file \"%s\", line %d\n", expr, file, line);

    while (true)
        SysLed::loop();         // blink
}