#pragma once

// TODO : à revoir

#include <string>

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Architecture::Register;
using namespace Architecture::Type;
using namespace Architecture::Name;
using namespace Architecture::InstructionCategory;

std::string to_string(Type type);
std::string to_string(Name name);
std::string to_string(const reg_t& reg, bool nice_names = true);
std::string to_string(InstructionCategory category);