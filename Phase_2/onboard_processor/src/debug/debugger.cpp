#ifndef __SYNTHESIS__

#include "debug/debugger.hpp"

#include <iostream>
#include <iomanip>
#include <string>

#include "debug/json.hpp"

uint64_t Debugger::cycles = 0;
json Debugger::report = {
	{ "Assembly", json::array() },
	{ "Cycles", json::object() }
};

void Debugger::new_cycle() {
	cycles++;
	report["Cycles"].push_back({ std::to_string(cycles), json::array() });
}

void Debugger::add_asm_line(std::string line) {
	report["Assembly"].push_back(line);
}

void Debugger::add_cycle_event(const json object) {
	json& current_cycle = report["Cycles"][std::to_string(cycles)];
	current_cycle.push_back(object);

	// std::cout << std::to_string(cycles) << " : " << object << '\n' << std::flush;
}

json Debugger::get_report() {
	return report;
}

#endif // __SYNTHESIS__