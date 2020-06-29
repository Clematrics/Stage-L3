#pragma once

#include "../include/common.hpp"
#include "../include/instructions.hpp"
#include "../include/register_file.hpp"

struct DecodedInstruction {
	Instruction::Name name;
	physical_id_t dest;
	physical_id_t reg1;
	physical_id_t reg2;
	word_t immediate;
};

word_t decodeIImmediate(const word_t& instruction);
word_t decodeSImmediate(const word_t& instruction);
word_t decodeBImmediate(const word_t& instruction);
word_t decodeUImmediate(const word_t& instruction);
word_t decodeJImmediate(const word_t& instruction);

class Decode {
	bool active;
	RegisterFile& register_file;
public:
	Decode(RegisterFile& register_file);
	void decode(const word_t& instruction, const word_t& program_counter, word_t* out_program_counter, bit_t* stop_signal, DecodedInstruction* decoded);
};
