/* ****************************************************************************
*    Source file containing some helpers for debugging purposes
**************************************************************************** */

#ifndef __SYNTHESIS__

#include "debug/debug_helpers.hpp"

#include <bitset>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#include "common.hpp"

int trailing_zeros(uint64_t n) {
    uint64_t v = n;
    int c = 64;
    v &= -signed(v);
    if (v) c--;
    if (v & 0x00000000FFFFFFFF) c -= 32;
    if (v & 0x0000FFFF0000FFFF) c -= 16;
    if (v & 0x00FF00FF00FF00FF) c -= 8;
    if (v & 0x0F0F0F0F0F0F0F0F) c -= 4;
    if (v & 0x3333333333333333) c -= 2;
    if (v & 0x5555555555555555) c -= 1;
    return c;
}

std::string string_hex(uint32_t n, uint8_t width) {
	std::ostringstream ss;
	ss << "0x" << std::hex << std::setw(width) << std::setfill('0') << n;
	return ss.str();
}

std::string string_all(uint32_t n) {
	std::ostringstream ss;
	ss << std::setw(10) << std::setfill(' ') << std::to_string(n) << " | " << string_hex(n) << " | " << string_bin(n);
	return ss.str();
}

#endif // __SYNTHESIS__