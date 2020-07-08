#pragma once

#include "architecture/architecture.hpp"

#include <string>

std::string to_string(Type type);
std::string to_string(Kind kind);
std::string to_string(Name name);
std::string to_string(const reg_t& reg, bool nice_names = true);