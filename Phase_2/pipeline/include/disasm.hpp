#pragma once


#ifndef __SYNTHESIS__

#include <ostream>
#include <string>

#include "../include/helpers.hpp"
#include "../include/instructions.hpp"
#include "../include/json.hpp"

struct AsmInstruction {
	word_t raw;
	opcode_t opcode;
	opcode_prefix_t opcode_prefix;
	opcode_suffix_t opcode_suffix;
	func3_t func3;
	func7_t func7;

	Instruction::Name name;
	Instruction::Type type;
	reg_t destination;
	reg_t source1;
	reg_t source2;
	word_t immediate;
};

const AsmInstruction disassemble(const word_t& raw);
std::string instruction_to_string(const AsmInstruction& instr);
json instruction_to_json(const AsmInstruction& instr);

#endif // __SYNTHESIS__