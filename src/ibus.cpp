#include <cassert>
#include <cstdint>
//
#include "misc/ibus.h"


// add a byte to a packet, return true if complete & valid packet
bool IBus::data(uint8_t byte)
{
    assert(0 <= _pkt_idx && _pkt_idx < pkt_len);
    if (_pkt_idx == 0) {
        if (byte != sync_0) {
            // _pkt_idx already 0
            return false;
        }
        _pkt[0] = byte;
        _pkt_idx = 1;
        return false;
    } else if (_pkt_idx == 1) {
        if (byte != sync_1) {
            _pkt_idx = 0;
            return false;
        }
        _pkt[1] = byte;
        _pkt_idx = 2;
        return false;
    } else {
        // channel data or checksum
        _pkt[_pkt_idx++] = byte;
        if (_pkt_idx != pkt_len)
            return false;
        // In theory, if the checksum fails we could have valid
        // sync bytes in the middle and should start there. Eh.
        _pkt_idx = 0;
        if (check_checksum()) {
            _data_us = time_us_64();
            return true;
        } else {
            return false;
        }
    }
}


// idx = 1...14
uint16_t IBus::channel(int idx)
{
    assert(0 < idx && idx <= channels);
    int offset = idx * 2;                     // 2, 4, 6, etc.
    return uint16_t(_pkt[offset]) +           // lsb
           (uint16_t(_pkt[offset + 1]) << 8); // msb
}


bool IBus::check_checksum()
{
    uint16_t calc = 0xffff;
    for (int i = 0; i < (pkt_len - 2); i++)
        calc -= _pkt[i];
    return (calc & 0xff) == _pkt[pkt_len - 2] &&
           ((calc >> 8) & 0xff) == _pkt[pkt_len - 1];
}
