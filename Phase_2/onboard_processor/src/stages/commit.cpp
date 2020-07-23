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
	reorder_buffer[reorder_buffer_top] = entry;
	reorder_buffer_top++;
	reorder_buffer_empty = false;
	if (reorder_buffer_bot == reorder_buffer_top)
		reorder_buffer_full = true;
}

void CommitStage::reorder_buffer_pop() {
	reorder_buffer_bot++;
	reorder_buffer_full = false;
	if (reorder_buffer_bot == reorder_buffer_top)
		reorder_buffer_empty = true;
}

void CommitStage::reorder_buffer_push_and_pop(ReorderBufferEntry new_entry) {
	reorder_buffer[reorder_buffer_top] = new_entry;
	reorder_buffer_bot++;
	reorder_buffer_top++;
}

ReorderBufferEntry CommitStage::reorder_buffer_first() {
	return reorder_buffer[reorder_buffer_bot];
}

bit_t CommitStage::reorder_buffer_is_empty() {
	return reorder_buffer_empty;
}

bit_t CommitStage::reorder_buffer_is_full() {
	return reorder_buffer_full;
}

ReorderBufferEntry& CommitStage::reorder_buffer_at(uint16_t index) {
	return reorder_buffer[index];
}

#ifndef __SYNTHESIS__
json CommitStage::reorder_buffer_as_json() {
	json array = json::array();
	for (uint32_t i = reorder_buffer_bot ; i != reorder_buffer_top ; i = (i + 1) % reorder_buffer_count)
		array.push_back({
			{ "Token",               reorder_buffer[i].token.to_uint()               },
			{ "Instruction done",    reorder_buffer[i].instruction_done.to_bool()    },
			{ "Invalid instruction", reorder_buffer[i].invalid_instruction.to_bool() }
		});
	return json({
		{ "Empty", reorder_buffer_empty.to_bool() },
		{ "Full",  reorder_buffer_empty.to_bool() },
		{ "List",  array           }
	});
}
#endif // __SYNTHESIS__

void CommitStage::interface(DecodeToCommit& from_decode, bit_t* stop, bit_t* commit_ran) {
	#pragma HLS inline
	#pragma HLS dependence variable=reorder_buffer inter false
	#pragma HLS data_pack  variable=reorder_buffer

	ReorderBufferEntry first_entry = reorder_buffer_first();
	ReorderBufferEntry new_entry; // Preparation of a potential new entry.
	new_entry.token               = from_decode.token;
	new_entry.instruction_done    = from_decode.invalid_instruction; // An invalid instruction does nothing, and so is immediately done
	new_entry.invalid_instruction = from_decode.invalid_instruction;

	// TODO : update the ROB with the information sent by the write back
	// idea : to avoid reading and writing at the bottom of the reorder_buffer,
	// if the token from the write_back is the same as the token from the top, just dequeue the instruction
	// otherwise, write the instruction with the same token as done

	bit_t can_dequeue = !reorder_buffer_is_empty() && first_entry.instruction_done;
	bit_t can_queue   = from_decode.add_to_rob;

	if (can_dequeue) {
		if (first_entry.invalid_instruction)
			*stop = true;
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

	// TODO : block if the ROB is full

	#ifndef __SYNTHESIS__
	json obj = json({});
	if (can_queue) {
		obj.push_back({ "Queued token",        new_entry.token.to_uint()               });
		obj.push_back({ "Invalid instruction", new_entry.invalid_instruction.to_bool() });
	}
	if (can_dequeue) {
		obj.push_back({ "Dequeued token",      first_entry.token.to_uint()               });
		obj.push_back({ "Invalid instruction", first_entry.invalid_instruction.to_bool() });
	}
	Debugger::add_cycle_event({
		{ "Commit stage",
			obj
		}
	});
	#endif // __SYNTHESIS__

	*commit_ran = true;
}

// void CommitStage::interface(DecodeToCommit& from_decode, bit_t* stop, bit_t* commit_ran) {
// 	#pragma HLS inline
// 	#pragma HLS dependence variable=reorder_buffer.list inter false

// 	// TODO : update the ROB with the information sent by the write back

// 	ReorderBufferEntry first_entry = reorder_buffer.first();
// 	ReorderBufferEntry new_entry; // Preparation of a potential new entry.
// 	new_entry.token               = from_decode.token;
// 	new_entry.instruction_done    = from_decode.invalid_instruction; // An invalid instruction does nothing, and so is immediately done
// 	new_entry.invalid_instruction = from_decode.invalid_instruction;

// 	bit_t can_dequeue = !reorder_buffer.is_empty() && first_entry.instruction_done;
// 	bit_t can_queue   = from_decode.add_to_rob;

// 	if (can_dequeue) {
// 		if (first_entry.invalid_instruction)
// 			*stop = true;
// 		// TODO : commit instruction, do all necessary work
// 	}

// 	/* There are four possible operations
// 	* operation = 0b00 -> nothing to do with the ROB
// 	* operation = 0b01 -> only dequeue from the ROB
// 	* operation = 0b00 -> only queue into the ROB
// 	* operation = 0b00 -> queue and dequeue simultaneously
// 	*/
// 	ap_uint<2> operation = (can_queue, can_dequeue);
// 	switch (operation) {
// 	case 0b00:                                         break;
// 	case 0b01: reorder_buffer.pop();                   break;
// 	case 0b10: reorder_buffer.push_back(new_entry);    break;
// 	case 0b11: reorder_buffer.push_and_pop(new_entry); break;
// 	}

// 	// TODO : block if the ROB is full

// 	#ifndef __SYNTHESIS__
// 	json obj = json({});
// 	if (can_queue) {
// 		obj.push_back({ "Queued token",        new_entry.token.to_uint()               });
// 		obj.push_back({ "Invalid instruction", new_entry.invalid_instruction.to_bool() });
// 	}
// 	if (can_dequeue) {
// 		obj.push_back({ "Dequeued token",      first_entry.token.to_uint()               });
// 		obj.push_back({ "Invalid instruction", first_entry.invalid_instruction.to_bool() });
// 	}
// 	Debugger::add_cycle_event({
// 		{ "Commit stage",
// 			obj
// 		}
// 	});
// 	#endif // __SYNTHESIS__

// 	*commit_ran = true;
// }