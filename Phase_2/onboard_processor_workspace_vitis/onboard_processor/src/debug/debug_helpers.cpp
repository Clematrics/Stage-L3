#include "debug/debug_helpers.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

std::string string_hex(uint32_t n, uint8_t width) {
	std::ostringstream ss;
	ss << "0x" << std::hex << std::setw(width) << std::setfill('0') << n;
	return ss.str();
}