#include "../include/commit.hpp"

Commit::Commit() {

}

void Commit::commit(DecodedInstruction instruction_in, bool* blocking, bool* ready, DecodedInstruction* instruction_out) {
	if (reorder_buffer.is_full()) {
		*blocking = true;
	}
	else {
		*blocking = false;
		reorder_buffer.push_back(instruction_in);
	}

	if (reorder_buffer.is_empty()) {
		*ready = false;
	}
	else {
		// *ready = indicate if instruction is ready
		// if so, pop instruction from reorder_buffer
	}
}
