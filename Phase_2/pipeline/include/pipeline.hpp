#pragma once

#include "../include/common.hpp"
#include "../include/fetch.hpp"
#include "../include/decode.hpp"
#include "../include/issue.hpp"
#include "../include/register_file.hpp"
#include "../include/reorder_buffer.hpp"

class Pipeline {
	Fetch fetch_stage;
	Decode decode_stage;
	// Issue issue_stage;
	// RegisterFile register_file;
	// ReorderBuffer reorder_buffer;
	DecodedInstruction decoded;
	word_t instruction;
	word_t program_counter;
	word_t next_program_counter;
public:
	Pipeline(word_t initial_program_counter = 0);
	void pipeline(memory_t memory, bit_t* stop);
};

void pipeline(memory_t memory);
