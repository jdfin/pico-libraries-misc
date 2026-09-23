#pragma once


template<typename T, int LEN>
class Ring {

  public:

    Ring() :
      _head(0),
      _tail(-1)
    {
    }

    ~Ring()
    {
      _head = 0;
      _tail = -1;
    }

    void reset()
    {
      _head = 0;
      _tail = -1;
    }

    bool empty() const
    {
      return _tail == -1;
    }

    bool full() const
    {
      return _head == _tail;
    }

    int count() const
    {
      if (_tail == -1)
        return 0;
      else if (_head == _tail)
        return LEN;
      else if (_head > _tail)
        return _head - _tail;
      else // _head < _tail
        return _head + LEN - _tail;
    }

    int length() const
    {
      return LEN;
    }

    void push(const T x)
    {
      _buf[_head] = x; // location at _head is always considered empty

      if (_tail == -1)
        // just became nonempty; set _tail to where we just put an item
        _tail = _head;
      else if (_tail == _head)
        // full; advance _tail (discarding one item)
        _tail = (_tail + 1) % LEN;

      _head = (_head + 1) % LEN;
    }

    T pop()
    {
      assert(_tail != -1);

      T x = _buf[_tail];

      _tail = (_tail + 1) % LEN;

      if (_tail == _head)
        _tail = -1;

      return x;
    }

  private:
    T _buf[LEN];

    // _tail == -1 means empty
    // _head == _tail means full (_len elements)
    // _head is always where the next item will go
    // _tail is the next item to be popped if it is not -1
    int _head;
    int _tail;
};
