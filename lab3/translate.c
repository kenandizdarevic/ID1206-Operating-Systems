#include <stdio.h>

const int pageOffsetMask = 0xFF;
const int pageNumberMask = 0xFF00;


// Read from addresses.txt, containg 32-bit integers.
// The numbers are representing logical addresses.
// Need to mask the rightmost 16 bits of each address.
// 8-bits page number, 8-bits page offset.

// Other specifics:
// 16 entries in TLB
// 256 entries in PT
// Page size 256 bytes
// 256 frames in physical memory
// Frame size 256 bytes
// Physical memory of 65 536 bytes

/// @brief Extracts bits 1-8, which is the page offset.
/// @param address 
/// @return int pageOffset
int pageOffset(int address) {
    return address & pageOffsetMask;
}

/// @brief Extracts bits 9-16, which is the page number.
/// @param address 
/// @return int pageNumber
int pageNumber(int address) {
    return (address & pageNumberMask) >> 8;
}

int main() {

    return 0;
}