#include "../include/reorder_buffer.hpp"

ReorderBuffer::ReorderBuffer()
{
	for (uint16_t i = 0; i < reorder_buffer_count; i++) {
		#pragma HLS UNROLL
		free_indices[i] = i;
	}
	free_indices_bot_ptr = 0;
	free_indices_top_ptr = 0;
}

void ReorderBuffer::add_instruction(DecodedInstruction instr, bool* blocking) {
	if (free_indices_bot_ptr == free_indices_top_ptr) {
		*blocking = true;
	}
	else {
		table[free_indices[free_indices_bot_ptr]] = instr;
		free_indices_bot_ptr++;
	}
}

void ReorderBuffer::take_instruction(bool* ready, DecodedInstruction* instr) {
	if (free_indices_bot_ptr == free_indices_top_ptr) {
		*ready = false;
	}
	else {
		*ready = true;
		reorder_buffer_ptr_t i = 0; // TODO : select a ready instruction
		*instr = table[i];
		free_indices[free_indices_top_ptr] = i;
		free_indices_top_ptr++;
	}
}