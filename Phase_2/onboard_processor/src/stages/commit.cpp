/* ****************************************************************************
*    Source file for the commit stage
**************************************************************************** */

#include "stages/commit.hpp"

#include "debug/debug.hpp"

CommitStage::CommitStage() {
	reorder_buffer_bot   = initial_token;
	reorder_buffer_top   = initial_token;
	reorder_buffer_empty = true;
	reorder_buffer_full  = false;
}

void CommitStage::reorder_buffer_pop() {
	#pragma HLS inline
	#pragma HLS array_partition variable=reorder_buffer complete

	reorder_buffer_bot++;
	reorder_buffer_full = false;
	if (reorder_buffer_bot == reorder_buffer_top)
		reorder_buffer_empty = true;
}

void CommitStage::reorder_buffer_push(ReorderBufferEntry entry) {
	#pragma HLS inline
	#pragma HLS array_partition variable=reorder_buffer complete

	reorder_buffer[reorder_buffer_top] = entry;
	reorder_buffer_top++;
	reorder_buffer_empty = false;
	if (reorder_buffer_bot == reorder_buffer_top)
		reorder_buffer_full = true;
}

void CommitStage::reorder_buffer_push_and_pop(ReorderBufferEntry new_entry) {
	#pragma HLS inline
	#pragma HLS array_partition variable=reorder_buffer complete

	reorder_buffer[reorder_buffer_top] = new_entry;
	reorder_buffer_bot++;
	reorder_buffer_top++;
}

#ifdef DBG_SYNTH
void CommitStage::interface(memory_t memory, const DecodeToCommit& from_decode, const WriteBackToCommit& from_write_back, const CommitToCommit& from_commit, CommitToIssue* to_issue, CommitToCommit* to_commit, bit_t* stop, CommitStatus* status) {
#else
void CommitStage::interface(memory_t memory, const DecodeToCommit& from_decode, const WriteBackToCommit& from_write_back, const CommitToCommit& from_commit, CommitToIssue* to_issue, CommitToCommit* to_commit, bit_t* stop) {
#endif
	#pragma HLS inline
	#pragma HLS array_partition variable=reorder_buffer complete

	// default output values
	to_issue->has_loaded_value = false;

	ReorderBufferEntry first_entry = from_commit.previous_first_entry; // Retrieve the copy of the first entry of the ROB.
	ReorderBufferEntry new_entry; // Preparation of a potential new entry.
	new_entry.token   = from_decode.token;
	new_entry.ready   = from_decode.invalid; // An invalid instruction does nothing, and so is marked as ready
	new_entry.invalid = from_decode.invalid;

	// TODO : update the ROB with the information sent by the write back
	// idea : to avoid reading and writing at the bottom of the reorder_buffer,
	// if the token from the write_back is the same as the token from the top, just dequeue the instruction
	// otherwise, write the instruction with the same token as done
	// Problem if updating the second entry ?
	if (from_write_back.executed_instruction) {
		ReorderBufferEntry& entry = reorder_buffer[from_write_back.token];

		entry.invalid              = from_write_back.invalid;
		entry.is_load              = from_write_back.is_load;
		entry.is_store             = from_write_back.is_store;
		entry.is_byte_operation    = from_write_back.is_byte_operation;
		entry.is_half_operation    = from_write_back.is_half_operation;
		entry.is_word_operation    = from_write_back.is_word_operation;
		entry.is_unsigned_extended = from_write_back.is_unsigned_extended;
		entry.load_dest            = from_write_back.load_dest;
		entry.address              = from_write_back.address;
		entry.value                = from_write_back.value;
		entry.ready = true;

	}

	bit_t can_dequeue = !from_commit.rob_was_empty && first_entry.ready;
	bit_t can_queue   = from_decode.add_to_rob;

	if (reorder_buffer_full) // TODO : remove
		*stop = true;
	if (can_dequeue) {
		if (first_entry.invalid) {
			*stop = true;
		}
		else {
			if (first_entry.is_load) {
				to_issue->has_loaded_value = true;
				to_issue->dest             = first_entry.load_dest;

				word_t value = memory[first_entry.address];
				// TODO : load values of different sizes
			}
			if (first_entry.is_store) {
				memory[first_entry.address] = first_entry.value;
				// TODO : store valeus of different sizes
			}
		}
		// TODO : commit instruction, do all necessary work
	}

	/* There are four possible operations
	* operation = 0b00 -> nothing to do with the ROB
	* operation = 0b01 -> only dequeue from the ROB
	* operation = 0b10 -> only queue into the ROB
	* operation = 0b11 -> queue and dequeue simultaneously
	*/
	ap_uint<2> operation = (can_queue, can_dequeue);
	switch (operation) {
	case 0b00:                                         break;
	case 0b01: reorder_buffer_pop();                   break;
	case 0b10: reorder_buffer_push(new_entry);         break;
	case 0b11: reorder_buffer_push_and_pop(new_entry); break;
	}

	to_commit->rob_was_empty        = reorder_buffer_empty;
	to_commit->previous_first_entry = reorder_buffer[reorder_buffer_bot]; // Create the copy of the first entry of the ROB.

	// TODO : block if the ROB is full

	#ifndef __SYNTHESIS__
	json decisions = json::object();
	if (can_queue) {
		decisions.push_back({ "Can queue", {
			{ "Queued token",        new_entry.token.to_uint()   },
			{ "Invalid instruction", new_entry.invalid.to_bool() }
		} });
	}
	if (can_dequeue) {
		decisions.push_back({ "Can dequeue", {
			{ "Dequeued token",      first_entry.token.to_uint()   },
			{ "Invalid instruction", first_entry.invalid.to_bool() }
		} });
	}

	json json_reorder_buffer = json::object();
	FOR_CYCLE_BUFFER(reorder_buffer, reorder_buffer_count) {
		json_reorder_buffer.push_back({ std::to_string(i), to_json(reorder_buffer[i]) });
	}

	Debugger::add_cycle_event("Commit stage", {
		{ "Reorder buffer",       json_reorder_buffer            },
		{ "Reorder buffer bot",   reorder_buffer_bot.to_uint()   },
		{ "Reorder buffer top",   reorder_buffer_top.to_uint()   },
		{ "Reorder buffer empty", reorder_buffer_empty.to_bool() },
		{ "Reorder buffer full",  reorder_buffer_full.to_bool()  },
		// Decisions
		{ "Decisions",            decisions                      }
	});
	#endif // __SYNTHESIS__

	#ifdef DBG_SYNTH
	for (uint16_t i = 0; i < reorder_buffer_count; i++)
		status->reorder_buffer[i] = reorder_buffer[i];
	status->reorder_buffer_bot    = reorder_buffer_bot;
	status->reorder_buffer_top    = reorder_buffer_top;
	status->reorder_buffer_empty  = reorder_buffer_empty;
	status->reorder_buffer_full   = reorder_buffer_full;
	#endif
}
