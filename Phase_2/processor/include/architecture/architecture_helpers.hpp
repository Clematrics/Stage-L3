#pragma once

#include <string>

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

std::string to_string(Type type);
std::string to_string(Decoding::Kind kind);
std::string to_string(Name name);
std::string to_string(const reg_t& reg, bool nice_names = true);