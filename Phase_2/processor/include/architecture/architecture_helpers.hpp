#pragma once

#include <string>

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Architecture::Register;
using namespace Architecture::Type;
using namespace Architecture::Name;
using namespace Decoding::Kind;

std::string to_string(Type type);
std::string to_string(Kind kind);
std::string to_string(Name name);
std::string to_string(const reg_t& reg, bool nice_names = true);