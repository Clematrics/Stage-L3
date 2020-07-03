#pragma once

#include "../include/common.hpp"
#include "../include/decode.hpp"

class ReorderBuffer {
	DecodedInstruction table[reorder_buffer_count];
	reorder_buffer_ptr_t free_indices[reorder_buffer_count];
	reorder_buffer_ptr_t free_indices_bot_ptr;
	reorder_buffer_ptr_t free_indices_top_ptr;
public:
	ReorderBuffer();
	void add_instruction(DecodedInstruction instr, bool* blocking);
	void take_instruction(bool* ready, DecodedInstruction* instr);
};