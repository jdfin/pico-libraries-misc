#pragma once

#include <ctype.h>
#include <stdlib.h>

#include <cassert>

// Arguments are separated by whitespace (isspace true). Each argument is a
// single character (isalpha true) or a signed [+-] integer (isdigit true).
// Multiple characters in a row become multiple arguments. Two arguments need
// not be separated by whitespace if it is clear where one ends and the next
// begins. For example, "L 3" and "L3" are both two arguments, 'L' and 3.
// Similarly, "L3C", "L3 C", and "L 3 C" are all three arguments, 'L', 3, and
// 'C'.

class Args
{

public:

    enum class Type {
        CHAR,
        INT,
    };

    struct Arg {
        Type t;
        union {
            char c;
            int i;
        };
    };

    Args(const char *line)
    {
        _argc = 0;
        const char *p = line;
        while (*p != '\0' && _argc < max_args) {
            while (isspace(*p))
                p++;
            if (*p == '\0')
                break;
            if (isalpha(*p)) {
                argv[_argc].t = Type::CHAR;
                argv[_argc].c = *p;
                p++;
            } else if (isdigit(*p) || *p == '-' || *p == '+') {
                argv[_argc].t = Type::INT;
                char *end;
                argv[_argc].i = strtol(p, &end, 0);
                p = end;
            } else {
                // unrecognized token
                break;
            }
            _argc++;
        }
    }

    int argc() const
    {
        return _argc;
    }

    // don't call with n >= _argc
    const Arg &operator[](int n) const
    {
        assert(n >= 0 && n < _argc);
        return argv[n];
    }

private:

    static constexpr int max_args = 10;
    Arg argv[max_args];
    int _argc;

}; // class Args
