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

void CommitStage::reorder_buffer_push_back(ReorderBufferEntry entry) {
	#pragma HLS inline
	reorder_buffer[reorder_buffer_top] = entry;
	reorder_buffer_top++;
	reorder_buffer_empty = false;
	if (reorder_buffer_bot == reorder_buffer_top)
		reorder_buffer_full = true;
}

void CommitStage::reorder_buffer_pop() {
	#pragma HLS inline
	reorder_buffer_bot++;
	reorder_buffer_full = false;
	if (reorder_buffer_bot == reorder_buffer_top)
		reorder_buffer_empty = true;
}

void CommitStage::reorder_buffer_push_and_pop(ReorderBufferEntry new_entry) {
	#pragma HLS inline
	reorder_buffer[reorder_buffer_top] = new_entry;
	reorder_buffer_bot++;
	reorder_buffer_top++;
}

#ifndef __SYNTHESIS__
json CommitStage::reorder_buffer_as_json() {
	json array = json::array();
	for (uint32_t i = reorder_buffer_bot ; i != reorder_buffer_top ; i = (i + 1) % reorder_buffer_count)
		array.push_back({
			{ "Token",               reorder_buffer[i].token.to_uint()   },
			{ "Instruction done",    reorder_buffer[i].done.to_bool()    },
			{ "Invalid instruction", reorder_buffer[i].invalid.to_bool() }
		});
	return json({
		{ "Empty", reorder_buffer_empty.to_bool() },
		{ "Full",  reorder_buffer_empty.to_bool() },
		{ "List",  array           }
	});
}
#endif // __SYNTHESIS__

void CommitStage::interface(DecodeToCommit& from_decode, CommitToCommit& from_commit, CommitToCommit* to_commit, bit_t* stop, bit_t* commit_ran) {
	#pragma HLS inline

	// Anticipate the load of the second entry to replace the first entry if this one is pulled from the ROB
	ReorderBufferEntry second_entry = reorder_buffer[reorder_buffer_bot + 1];

	ReorderBufferEntry first_entry = from_commit.previous_first_entry; // Retrieve the copy of the first entry of the ROB.
	ReorderBufferEntry new_entry; // Preparation of a potential new entry.
	new_entry.token   = from_decode.token;
	new_entry.done    = from_decode.invalid; // An invalid instruction does nothing, and so is marked as done
	new_entry.invalid = from_decode.invalid;

	// TODO : update the ROB with the information sent by the write back
	// idea : to avoid reading and writing at the bottom of the reorder_buffer,
	// if the token from the write_back is the same as the token from the top, just dequeue the instruction
	// otherwise, write the instruction with the same token as done
	// Problem if updating the second entry ?

	bit_t can_dequeue = !from_commit.rob_was_empty && first_entry.done;
	bit_t can_queue   = from_decode.add_to_rob;

	if (reorder_buffer_full)
		*stop = true;
	if (can_dequeue) {
		// if (first_entry.invalid)
		// 	*stop = true;
		// TODO : commit instruction, do all necessary work
	}

	/* There are four possible operations
	* operation = 0b00 -> nothing to do with the ROB
	* operation = 0b01 -> only dequeue from the ROB
	* operation = 0b00 -> only queue into the ROB
	* operation = 0b00 -> queue and dequeue simultaneously
	*/
	ap_uint<2> operation = (can_queue, can_dequeue);
	switch (operation) {
	case 0b00:                                         break;
	case 0b01: reorder_buffer_pop();                   break;
	case 0b10: reorder_buffer_push_back(new_entry);    break;
	case 0b11: reorder_buffer_push_and_pop(new_entry); break;
	}

	to_commit->rob_was_empty        = reorder_buffer_empty;
	to_commit->previous_first_entry = can_dequeue ? second_entry : first_entry; // Create the copy of the first entry of the ROB.

	// TODO : block if the ROB is full

	#ifndef __SYNTHESIS__
	json obj = json({});
	json array = json::array();
	for (uint32_t i = reorder_buffer_bot, do_loop = !reorder_buffer_empty ; i != reorder_buffer_top || do_loop ; i = (i + 1) % reorder_buffer_count, do_loop = false)
		array.push_back({ std::to_string(i - reorder_buffer_bot),
			{
				{ "Token",   reorder_buffer[i].token.to_uint()   },
				{ "Done",    reorder_buffer[i].done.to_bool()    },
				{ "Invalid", reorder_buffer[i].invalid.to_bool() },
			}
		});
	obj.push_back({ "ROB", array });
	if (can_queue) {
		obj.push_back({ "Queued token",        new_entry.token.to_uint()   });
		obj.push_back({ "Invalid instruction", new_entry.invalid.to_bool() });
	}
	if (can_dequeue) {
		obj.push_back({ "Dequeued token",      first_entry.token.to_uint()   });
		obj.push_back({ "Invalid instruction", first_entry.invalid.to_bool() });
	}
	obj.push_back({ "To commit",
		{
			{ "ROB was empty",          to_commit->rob_was_empty.to_bool()                },
			{ "First entry token",      to_commit->previous_first_entry.token.to_uint()   },
			{ "First entry is done",    to_commit->previous_first_entry.done.to_bool()    },
			{ "First entry is invalid", to_commit->previous_first_entry.invalid.to_bool() }
		}
	});
	Debugger::add_cycle_event({
		{ "Commit stage",
			obj
		}
	});
	#endif // __SYNTHESIS__

	*commit_ran = true;
}
