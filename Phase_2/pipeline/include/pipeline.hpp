#pragma once

#include "../include/common.hpp"
#include "../include/fetch.hpp"
#include "../include/decode.hpp"
#include "../include/register_file.hpp"

class Pipeline {
	Fetch fetch_stage;
	Decode decode_stage;
	RegisterFile register_file;
	DecodedInstruction decoded;
	word_t instruction;
	word_t program_counter;
	word_t next_program_counter;
public:
	Pipeline(word_t initial_program_counter = 0);
	void pipeline(memory_t memory, bit_t* stop);
};

void pipeline(memory_t memory);
