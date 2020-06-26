#include "../include/helpers.hpp"

#include <bitset>
#include <iomanip>
#include <string>
#include <sstream>

#include "../include/common.hpp"

std::string string_bin(uint32_t n) {
	std::ostringstream ss;
	ss << "0x" << std::hex << n;
	return ss.str();
}

std::string string_hex(uint32_t n) {
	std::ostringstream ss;
	ss << "0b" << std::bitset<32>(n);
	return ss.str();
}