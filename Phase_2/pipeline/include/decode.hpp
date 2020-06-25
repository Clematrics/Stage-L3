#pragma once

#include "../include/common.hpp"
#include "../include/instructions.hpp"

#define ITERATION5

struct DecodedInstruction {
	Instruction::Name name;
	reg_t dest;
	reg_t reg1;
	reg_t reg2;
	word_t immediate;
};

class Decode {
	#ifdef OTHER
	InstructionType instructionType(const word_t instruction);
	#endif
	bool active;
public:
	Decode();
	void decode(const word_t instruction, word_t program_counter, word_t* out_program_counter, bit_t* stop_signal, DecodedInstruction* decoded);
};
