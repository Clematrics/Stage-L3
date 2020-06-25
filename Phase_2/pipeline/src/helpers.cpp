#include "../include/helpers.hpp"

#include <bitset>
#include <iomanip>
#include <string>
#include <sstream>

std::string string_bin(unsigned int n) {
	std::ostringstream ss;
	ss << "0x" << std::hex << n;
	return ss.str();
}

std::string string_hex(unsigned int n) {
	std::ostringstream ss;
	ss << "0b" << std::bitset<32>(n);
	return ss.str();
}