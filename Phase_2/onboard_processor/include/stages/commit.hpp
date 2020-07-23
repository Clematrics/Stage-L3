#pragma once

/* ****************************************************************************
*    Header file for the commit stage
**************************************************************************** */

#include "common.hpp"
#include "components/circular_buffer.hpp"
#include "inter_stage.hpp"

struct ReorderBufferEntry {
	instruction_token_t token; // is not usefull, since the token is the same as the entry's index in the ROB
	bit_t               instruction_done;
	bit_t               invalid_instruction;
};

class CommitStage {
	// CircularBuffer<ReorderBufferEntry, reorder_buffer_count, reorder_buffer_ptr_t, initial_token> reorder_buffer;
	ReorderBufferEntry   reorder_buffer[reorder_buffer_count];
	reorder_buffer_ptr_t reorder_buffer_bot;
	reorder_buffer_ptr_t reorder_buffer_top;
	reorder_buffer_ptr_t reorder_buffer_empty;
	reorder_buffer_ptr_t reorder_buffer_full;

	void reorder_buffer_push_back(ReorderBufferEntry entry);
	void reorder_buffer_pop();
	void reorder_buffer_push_and_pop(ReorderBufferEntry new_entry);
	ReorderBufferEntry reorder_buffer_first();
	bit_t reorder_buffer_is_empty();
	bit_t reorder_buffer_is_full();
	ReorderBufferEntry& reorder_buffer_at(uint16_t index);
	#ifndef __SYNTHESIS__
	json reorder_buffer_as_json();
	#endif // __SYNTHESIS__
public:
	CommitStage();
	void interface(DecodeToCommit& from_decode, bit_t* stop, bit_t* commit_ran);
};