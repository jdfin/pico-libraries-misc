#pragma once

class Tokens {

public:
    Tokens();

    void reset();
    void print() const;
    void eat(int cnt = tokens_cnt_max);

    void add_char(char c);

    int count() const { return _tokens_cnt; }

    const char* operator[](int idx) const;

    int verbosity() const { return _verbosity; }

    void verbosity(int v) { _verbosity = v; }

private:
    static const char escape = 0x1b;

    static const int token_len_max = 10;

    int _verbosity;

    // The "input" is consecutive non-whitespace characters. When any
    // whitespace is found, the accumulating input line becomes the next
    // token.
    static const int input_max = token_len_max;
    char _input[input_max];
    int _input_cnt;

    // A command is a few consecutive tokens.
    static const int tokens_cnt_max = 10; // max number of tokens
    char _tokens[tokens_cnt_max][token_len_max];
    int _tokens_cnt = 0; // number of tokens in array

    void add_token();

}; // class Tokens