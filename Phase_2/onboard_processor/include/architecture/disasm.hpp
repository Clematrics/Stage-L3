#pragma once

// TODO à revoir complétement pour utiliser les infos extraites par le décodeur

#ifndef __SYNTHESIS__

#include <ostream>
#include <string>

#include "architecture.hpp"
#include "decoding.hpp"
#include "debug/debug.hpp"
#include "architecture_helpers.hpp"

struct DisassembledInstruction {
	word_t raw;
	opcode_t opcode;
	opcode_prefix_t opcode_prefix;
	opcode_suffix_t opcode_suffix;
	func3_t func3;
	func7_t func7;

	Name name;
	Type type;
	reg_t destination;
	reg_t source1;
	reg_t source2;
	word_t immediate;
};

const DisassembledInstruction disassemble(const word_t& raw);
Name get_instruction_name(const word_t& instruction, const opcode_prefix_t& opcode_prefix, const opcode_suffix_high_t& opcode_high, const opcode_suffix_low_t& opcode_low, const func3_t& func3, const func7_t& func7);
std::string instruction_to_string(const DisassembledInstruction& instr);
json instruction_to_json(const DisassembledInstruction& instr);

#endif // __SYNTHESIS__