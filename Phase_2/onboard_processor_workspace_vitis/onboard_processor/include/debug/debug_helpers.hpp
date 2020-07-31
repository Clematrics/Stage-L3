#pragma once

#include <bitset>
#include <cstdint>
#include <string>
#include <sstream>

template<int width = 32>
std::string string_bin(uint32_t n) {
	std::ostringstream ss;
	ss << "0b" << std::bitset<width>(n);
	return ss.str();
}

std::string string_hex(uint32_t n, uint8_t width = 32 / 4);