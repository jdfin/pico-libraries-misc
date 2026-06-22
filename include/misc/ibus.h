#pragma once

#include <cassert>
#include <cstdint>

// 32-byte packets
// Little-endian
//
// Bytes Description  Meaning
// 0     Sync         0x20 (first byte)
// 1     Sync         0x40 (second byte)
// 2-29  Channel Data 28 bytes containing up to 14 RC channels
// 30-31 Checksum     2 bytes used to verify data transmission integrity
//
// Each channel value typically ranges from 1000 to 2000.

class IBus
{

public:

    IBus() :
        _pkt_idx(0)
    {
    }

    // add a byte to a packet, return true if complete & valid packet
    bool data(uint8_t byte);

    static constexpr int channels = 14;

    // this is valid after data() returns true, and before it is called again
    uint16_t channel(int idx);

private:

    static constexpr uint8_t sync_0 = 0x20;
    static constexpr uint8_t sync_1 = 0x40;

    static constexpr int pkt_len = 32;
    uint8_t _pkt[pkt_len];
    int _pkt_idx;

    // (65535 - sum(_pkt[0-29])) should equal the uint16_t in _pkt[30-31]
    bool check_checksum();

}; // class IBus
