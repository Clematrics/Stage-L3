#pragma once

/* ****************************************************************************
*    Header file for the commit stage
**************************************************************************** */

#include "common.hpp"
#include "components/circular_buffer.hpp"
#include "stage_structs.hpp"

class CommitStage {
	// CircularBuffer<ReorderBufferEntry, reorder_buffer_count, reorder_buffer_ptr_t, initial_token> reorder_buffer;
	ReorderBufferEntry   reorder_buffer[reorder_buffer_count];
	reorder_buffer_ptr_t reorder_buffer_bot;
	reorder_buffer_ptr_t reorder_buffer_top;
	bit_t                reorder_buffer_empty;
	bit_t                reorder_buffer_full;

	void reorder_buffer_push_back(ReorderBufferEntry entry);
	void reorder_buffer_pop();
	void reorder_buffer_push_and_pop(ReorderBufferEntry new_entry);
	ReorderBufferEntry reorder_buffer_first();
	#ifndef __SYNTHESIS__
	json reorder_buffer_as_json();
	#endif // __SYNTHESIS__

	// this stream contains at his head a copy of the first entry of the reorder buffer at the end of the cycle
	// this avoids to access dynamically the ROB at the beginning of the cycle and to get a RAW dependency

public:
	CommitStage();
	void interface(DecodeToCommit& from_decode, CommitToCommit& from_commit, CommitToCommit* to_commit, bit_t* stop, bit_t* commit_ran);
};