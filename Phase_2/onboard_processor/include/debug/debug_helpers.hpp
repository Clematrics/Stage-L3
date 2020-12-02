#pragma once

/* ****************************************************************************
*    Header file containing some helpers for debugging purposes
**************************************************************************** */

#ifndef __SYNTHESIS__

#include <bitset>
#include <iostream>
#include <string>
#include <sstream>

#include "common.hpp"

#define FOR_CYCLE_BUFFER(base, count) \
	for (uint32_t i = base ## _bot, do_loop = !base ## _empty ; \
	     i != base ## _top || do_loop ; \
		 i = (i + 1) % count, do_loop = false)

int trailing_zeros(uint64_t n);

template<int N = 32>
std::string string_bin(uint64_t n, bool spaced = false) {
    std::ostringstream ss;
    ss << "0b";
    std::bitset<N> bs(n);
    if (spaced) {
        for (int i = N - 1; i >= 1; i--) {
            // Groups of bits are packed by four. Groups are packed by 2.
            // Two groups are separated by more and more spaces, with respect to their depth in the tree of groups.
            // For instance :
            // 0000 0000  0000 0000   0000 0000  0000 0000    0000 0000  0000 0000   0000 0000  0000 0000
            // The number of spaces is given by the number of trailing zeros of (i / 4) since groups are formed of 4 bits
            ss << bs[i];
            if (!(i & 0b11)) {
                int tz = trailing_zeros(i >> 2);
                for (int j = 0; j <= tz; j++) ss << ' ';
            }
        }
        ss << bs[0];
    }
    else {
    	ss << bs;
    }
	return ss.str();
}

std::string string_hex(uint32_t n, uint8_t width = 32 / 4);

std::string string_all(uint32_t n);

#endif // __SYNTHESIS__