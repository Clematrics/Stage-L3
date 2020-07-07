#pragma once

#include "../include/common.hpp"
#include "../include/circular_buffer.hpp"
#include "../include/decode.hpp"

class Commit {
	CircularBuffer<DecodedInstruction, reorder_buffer_count, reorder_buffer_ptr_t> reorder_buffer;
public:
	Commit();
	void commit(DecodedInstruction instruction_in, bool* blocking, bool* ready, DecodedInstruction* instruction_out);
};