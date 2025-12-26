#pragma once

#include <cstdio>

class BufLog
{

public:

    static constexpr int line_len = 80;

    // get a line to log data into
    static char *write_line_get()
    {
        // see if log is full
        int put_next = _put + 1;
        if (put_next == line_cnt)
            put_next = 0;
        if (_get == put_next)
            return nullptr; // yes
        return _log[_put];  // no
    }

    // call this when done writing line
    // read_line_get can see the new line after calling this
    static void write_line_put()
    {
        if (++_put >= line_cnt)
            _put = 0;
    }

    // get line to (e.g.) print to console
    // call read_line_put to free it up after printing
    static const char *read_line_get()
    {
        if (_get == _put)
            return nullptr;
        return _log[_get];
    }

    // call this when done with read line
    // write_line_get can reuse the line after calling this
    static void read_line_put()
    {
        if (++_get >= line_cnt)
            _get = 0;
    }

    static void loop()
    {
        const char *p = read_line_get();
        if (p != nullptr) {
            printf("%s\n", p);
            read_line_put();
        }
    }

private:

    static constexpr int line_cnt = 16;

    static char _log[line_cnt][line_len];

    // _put == _get means empty
    // _get == _put + 1 means full
    static int _put;
    static int _get;

}; // class BufLog
