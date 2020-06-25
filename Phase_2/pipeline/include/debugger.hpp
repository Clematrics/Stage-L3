#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

class Debugger {
	static uint64_t cycles;
	static json report;
	static std::vector<std::string> assembly;
public:
	static void new_cycle();
	static void add_asm_line(std::string& line);
	static void add_event(json object);
	static json get_report();
	static std::vector<std::string>& get_assembly();
};