
#include <cassert>
#include <cstdio>

#include "args.h"
#include "pico/stdlib.h"
#include "sys_led.h"


static void args_test()
{
    // empty string
    {
        Args a("");
        assert(a.argc() == 0);
    }

    // whitespace only
    {
        Args a("   \t  ");
        assert(a.argc() == 0);
    }

    // single char arg
    {
        Args a("L");
        assert(a.argc() == 1);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'L');
    }

    // single int arg
    {
        Args a("3");
        assert(a.argc() == 1);
        assert(a[0].t == Args::Type::INT);
        assert(a[0].i == 3);
    }

    // negative int
    {
        Args a("-5");
        assert(a.argc() == 1);
        assert(a[0].t == Args::Type::INT);
        assert(a[0].i == -5);
    }

    // positive sign
    {
        Args a("+7");
        assert(a.argc() == 1);
        assert(a[0].t == Args::Type::INT);
        assert(a[0].i == 7);
    }

    // multi-digit integer
    {
        Args a("123");
        assert(a.argc() == 1);
        assert(a[0].t == Args::Type::INT);
        assert(a[0].i == 123);
    }

    // hex integer (base 0 strtol)
    {
        Args a("0x1F");
        assert(a.argc() == 1);
        assert(a[0].t == Args::Type::INT);
        assert(a[0].i == 31);
    }

    // multiple chars → multiple CHAR args: "ABC"
    {
        Args a("ABC");
        assert(a.argc() == 3);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'A');
        assert(a[1].t == Args::Type::CHAR);
        assert(a[1].c == 'B');
        assert(a[2].t == Args::Type::CHAR);
        assert(a[2].c == 'C');
    }

    // char then int with space: "L 3"
    {
        Args a("L 3");
        assert(a.argc() == 2);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'L');
        assert(a[1].t == Args::Type::INT);
        assert(a[1].i == 3);
    }

    // char then int no space: "L3"
    {
        Args a("L3");
        assert(a.argc() == 2);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'L');
        assert(a[1].t == Args::Type::INT);
        assert(a[1].i == 3);
    }

    // char, int, char no space: "L3C"
    {
        Args a("L3C");
        assert(a.argc() == 3);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'L');
        assert(a[1].t == Args::Type::INT);
        assert(a[1].i == 3);
        assert(a[2].t == Args::Type::CHAR);
        assert(a[2].c == 'C');
    }

    // same as above with spaces: "L 3 C"
    {
        Args a("L 3 C");
        assert(a.argc() == 3);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'L');
        assert(a[1].t == Args::Type::INT);
        assert(a[1].i == 3);
        assert(a[2].t == Args::Type::CHAR);
        assert(a[2].c == 'C');
    }

    // int followed by char no space: "-5C"
    {
        Args a("-5C");
        assert(a.argc() == 2);
        assert(a[0].t == Args::Type::INT);
        assert(a[0].i == -5);
        assert(a[1].t == Args::Type::CHAR);
        assert(a[1].c == 'C');
    }

    // char then multi-digit int: "L123"
    {
        Args a("L123");
        assert(a.argc() == 2);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'L');
        assert(a[1].t == Args::Type::INT);
        assert(a[1].i == 123);
    }

    // leading and trailing whitespace
    {
        Args a("  L  3  ");
        assert(a.argc() == 2);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'L');
        assert(a[1].t == Args::Type::INT);
        assert(a[1].i == 3);
    }

    // unrecognized token stops parsing
    {
        Args a("L.3");
        assert(a.argc() == 1);
        assert(a[0].t == Args::Type::CHAR);
        assert(a[0].c == 'L');
    }

    // unrecognized token at start: nothing parsed
    {
        Args a(".L3");
        assert(a.argc() == 0);
    }
}


int main()
{
    stdio_init_all();

    SysLed::init();
    SysLed::pattern(50, 950);

    while (!stdio_usb_connected()) {
        tight_loop_contents();
        SysLed::loop();
    }

    sleep_ms(10);

    args_test();

    printf("all tests passed\n");

    SysLed::pattern(950, 50);
    while (true)
        SysLed::loop();

    return 0;
}
