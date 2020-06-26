#pragma once

#include "../include/common.hpp"
#include "../include/instructions.hpp"

struct DecodedInstruction {
	Instruction::Name name;
	reg_t dest;
	reg_t reg1;
	reg_t reg2;
	word_t immediate;
};

word_t decodeIImmediate(const word_t& instruction);
word_t decodeSImmediate(const word_t& instruction);
word_t decodeBImmediate(const word_t& instruction);
word_t decodeUImmediate(const word_t& instruction);
word_t decodeJImmediate(const word_t& instruction);

class Decode {
	bool active;
public:
	Decode();
	void decode(const word_t& instruction, const word_t& program_counter, word_t* out_program_counter, bit_t* stop_signal, DecodedInstruction* decoded);
};
