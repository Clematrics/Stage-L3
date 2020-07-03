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
	packed_immediate_t packed_immediate;
};

word_t decode_I_immediate(const word_t& instruction);
word_t decode_S_immediate(const word_t& instruction);
word_t decode_B_immediate(const word_t& instruction);
word_t decode_U_immediate(const word_t& instruction);
word_t decode_J_immediate(const word_t& instruction);

packed_immediate_t pack_I_immediate(const word_t& instruction);
packed_immediate_t pack_S_immediate(const word_t& instruction);
packed_immediate_t pack_B_immediate(const word_t& instruction);
packed_immediate_t pack_U_immediate(const word_t& instruction);
packed_immediate_t pack_J_immediate(const word_t& instruction);

word_t unpack_I_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_S_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_B_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_U_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_J_immediate(const packed_immediate_t& packed_immediate);

word_t unpack_immediate(Instruction::Type type, const packed_immediate_t& packed_immediate);

class Decode {
	bool active;
	RegisterFile& register_file;
public:
	Decode(RegisterFile& register_file);
	void decode(const word_t& instruction, const word_t& program_counter, word_t* out_program_counter, bit_t* stop_signal, DecodedInstruction* decoded);
};
