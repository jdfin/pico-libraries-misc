#pragma once

class Argv {

public:
    Argv(int verbosity=0);

    void reset();

    void print() const;

    bool add_char(char c);
    bool add_str(const char *s);

    int argc() const;

    const char* operator[](int idx) const;

    int verbosity() const { return _verbosity; }

    void verbosity(int v) { _verbosity = v; }

    bool check(int line_cnt, const char *line, bool complete) const;

    int line_max() const { return _line_max; }

private:
    static const char escape = 0x1b;

    static const int _line_max = 80;

    int _verbosity;

    char _line[_line_max];
    int _line_cnt;

    // When a '\r' or '\n' is found, the input is a complete command line.
    bool _complete;

    void invariant() const;

}; // class Argv

/*

_line_cnt is always where the next input character will go.
It must always be < _line_max.

*****

after reset:
_line     [ ... ]
_line_cnt 0 ^
_complete false

after one non-whitespace character:
_line     [ X ... ]
_line_cnt 1   ^
_complete false

then another:
_line     [ X Y ... ]
_line_cnt 2     ^
_complete false

then a return:
_line     [ X Y 0 ... ]
_line_cnt 3       ^
_complete true

***** reset, two tokens, return:

reset:
_line     [ ... ]
_line_cnt 0 ^
_complete false

non-whitespace:
_line     [ X ... ]
_line_cnt 1   ^
_complete false

non-whitespace:
_line     [ X Y ... ]
_line_cnt 2     ^
_complete false

whitespace:
_line     [ X Y 0 ... ]
_line_cnt 3       ^
_complete false

non-whitespace:
_line     [ X Y 0 A ... ]
_line_cnt 4         ^
_complete false

newline:
_line     [ X Y 0 A 0 ... ]
_line_cnt 5           ^
_complete true

*/