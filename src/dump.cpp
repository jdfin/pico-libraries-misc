
#include <cstdint>
#include <cstdio>
#include "dump.h"

using std::uintptr_t;

void dump(const void *mem, int num_bytes)
{
    const unsigned char *ucm = (const unsigned char *)mem;
    bool new_line = true;

    while (num_bytes-- > 0) {
        // If we've just started a new line, print the address
        // The first time, it will be 'mem'.
        // After that, it will be a multiple of 16 bytes.
        if (new_line) {
            printf("%p:", mem);
            new_line = false;
        }
        printf(" %02x", unsigned(*(unsigned char *)ucm));
        ucm++;
        // if ucm is now on a 16-byte boundary, print a newline
        if ((reinterpret_cast<uintptr_t>(ucm) & 0xf) == 0) {
            printf("\n");
            new_line = true;
        }
    }
    // if the data did not end at a 16-byte boundary, print new line
    if (!new_line)
        printf("\n");
}