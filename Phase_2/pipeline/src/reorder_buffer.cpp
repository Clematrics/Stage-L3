#include "../include/reorder_buffer.hpp"

ReorderBuffer::ReorderBuffer()
{
}

void ReorderBuffer::add_instruction(DecodedInstruction instr, bool* blocking) {
	if (instr_list.is_full()) {
		*blocking = true;
	}
	else {
		*blocking = false;
		instr_list.push_back(instr);
	}
}

void ReorderBuffer::take_instruction(bool* ready, DecodedInstruction* instr) {
	if (instr_list.is_empty()) {
		*ready = false;
	}
	else {
		// *ready = indicate if instruction is ready
		// if so, pop instruction from instr_list
	}
}