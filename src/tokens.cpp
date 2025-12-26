#include "tokens.h"
#include "pico/stdlib.h"
#include "xassert.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctype.h>

Tokens::Tokens()
    : _verbosity(1)
{
    reset();
}

void Tokens::reset()
{
    _tokens_cnt = 0;
    _input_cnt = 0;
    _input[0] = '\0';
}

void Tokens::print() const
{
    if (_tokens_cnt == 0) {
        printf("%d tokens\n", _tokens_cnt);
    } else {
        if (_tokens_cnt == 1)
            printf("%d token:", _tokens_cnt);
        else
            printf("%d tokens:", _tokens_cnt);
        for (int i = 0; i < _tokens_cnt; i++)
            printf(" \"%s\"", _tokens[i]);
        printf("\n");
    }
}

void Tokens::eat(int cnt)
{
    xassert(cnt <= tokens_cnt_max);

    if (cnt > _tokens_cnt)
        cnt = _tokens_cnt;

    for (int i = 0; i < (_tokens_cnt - cnt); i++) {
        xassert(strlen(_tokens[i + cnt]) < token_len_max);
        strcpy(_tokens[i], _tokens[i + cnt]);
    }

    _tokens_cnt -= cnt;

    // print();
}

// copy input buffer to next token slot
void Tokens::add_token()
{
    xassert(strlen(_input) < input_max);
    // should not have more than max tokens, but if we do, take them fifo
    if (_tokens_cnt == tokens_cnt_max)
        eat(1);
    strcpy(_tokens[_tokens_cnt], _input);
    _tokens_cnt++;
    _input_cnt = 0;
    _input[0] = '\0';
}

// Build up new tokens in the input buffer, and when whitespace is found
// move the input buffer to the tokens array.
void Tokens::add_char(char c)
{
    xassert(_input_cnt < input_max);
    xassert(_input[_input_cnt] == '\0');

    if (c == escape) {
        reset();
        if (_verbosity > 0) {
            printf("\n");
            printf("input reset\n");
        }
        return;
    }

    if (isspace(c)) {
        // whitespace
        // create a new token if there is non-whitespace in the input buffer
        if (_input_cnt != 0) {
            add_token();
            if (_verbosity > 0)
                printf(" ");
        }
    } else {
        if (_verbosity > 0)
            printf("%c", toupper(c)); // echo
        // append to input buffer
        _input[_input_cnt] = toupper(c);
        _input_cnt++;
        if (_input_cnt >= input_max) {
            printf(" >>> input line too long (max %u chars)\n", input_max);
            reset();
        } else {
            // input buffer is always terminated
            _input[_input_cnt] = '\0';
        }
    }
}

const char* Tokens::operator[](int idx) const
{
    xassert(idx >= 0 && idx < _tokens_cnt);
    return _tokens[idx];
}