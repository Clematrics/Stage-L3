#pragma once

#include "../include/common.hpp"
#include "../include/circular_buffer.hpp"
#include "../include/decode.hpp"

class ReorderBuffer {
	CircularBuffer<DecodedInstruction, reorder_buffer_count, reorder_buffer_ptr_t> instr_list;
public:
	ReorderBuffer();
	void add_instruction(DecodedInstruction instr, bool* blocking);
	void take_instruction(bool* ready, DecodedInstruction* instr);
};