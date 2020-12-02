#pragma once

/* ****************************************************************************
*    Header file for the commit stage
**************************************************************************** */

#include "config.hpp"
#include "common.hpp"
#include "debug/debug.hpp"
#include "stage_structs.hpp"

class CommitStage {
	ReorderBufferEntry   reorder_buffer[reorder_buffer_count];
	reorder_buffer_ptr_t reorder_buffer_bot;
	reorder_buffer_ptr_t reorder_buffer_top;
	bit_t                reorder_buffer_empty;
	bit_t                reorder_buffer_full;

	void reorder_buffer_pop();
	void reorder_buffer_push(ReorderBufferEntry entry);
	void reorder_buffer_push_and_pop(ReorderBufferEntry new_entry);

public:
	CommitStage();
	#ifdef DBG_SYNTH
	void interface(memory_t memory, const DecodeToCommit& from_decode, const WriteBackToCommit& from_write_back, const CommitToCommit& from_commit, CommitToIssue* to_issue, CommitToCommit* to_commit, bit_t* stop, CommitStatus* status);
	#else
	void interface(memory_t memory, const DecodeToCommit& from_decode, const WriteBackToCommit& from_write_back, const CommitToCommit& from_commit, CommitToIssue* to_issue, CommitToCommit* to_commit, bit_t* stop);
	#endif
};