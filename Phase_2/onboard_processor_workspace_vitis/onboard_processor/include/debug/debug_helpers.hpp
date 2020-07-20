#pragma once

#include <cstdint>
#include <string>

std::string string_bin(uint32_t n);
std::string string_hex(uint32_t n, uint8_t width = 32 / 4);