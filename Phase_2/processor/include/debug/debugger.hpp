#pragma once

#ifndef __SYNTHESIS__

#include <string>

#include "common.hpp"
#include "debug/json.hpp"

class Debugger {
	static uint64_t cycles;
	static json report;
public:
	static void new_cycle();
	static void add_asm_line(std::string line);
	static void add_cycle_event(json object);
	static json get_report();
};

#endif // __SYNTHESIS__