#include "../include/debugger.hpp"

#include <string>

uint64_t Debugger::cycles = 0;
json Debugger::report = json::array();
std::vector<std::string> Debugger::assembly;

void Debugger::new_cycle() {
	cycles++;
	report.push_back({{"Cycle " + std::to_string(cycles), json::array()}});
}

void Debugger::add_asm_line(std::string& line) {
	assembly.push_back(line);
}

void Debugger::add_event(json object) {
	report.back().back().push_back(object);
}

json Debugger::get_report() {
	return report;
}

std::vector<std::string>& Debugger::get_assembly() {
	return assembly;
}