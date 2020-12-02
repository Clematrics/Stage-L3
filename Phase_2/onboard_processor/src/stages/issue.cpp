/* ****************************************************************************
*    Source file for the issue stage
**************************************************************************** */

#include "stages/issue.hpp"
#include "stages/stage_structs.hpp"

#include "debug/debug.hpp"

IssueStage::IssueStage() {
	#pragma HLS array_partition variable=register_file complete
	#pragma HLS array_partition variable=issue_table   complete
	#pragma HLS array_partition variable=free_entries  complete

	// Initializing register file
	for (uint16_t id = 0; id < physical_register_count; id++) {
		register_file[id] = 0;
	}

	// Initializing issue table
	for (uint16_t id = 0; id < issue_table_entries_count; id++) {
		issue_table[id] = (IssueTableEntry){ false };
	}

	// Initializing free entries
	for (uint16_t id = 0; id < issue_table_entries_count; id++) {
		free_entries[id] = id;
	}
	free_entries_bot = 0;
	free_entries_top = 0;
	free_entries_empty = false;
	free_entries_full  = true;
}

void IssueStage::free_entries_pop() {
	#pragma HLS inline
	free_entries_bot++;
	free_entries_full = false;
	if (free_entries_bot == free_entries_top)
		free_entries_empty = true;
}

void IssueStage::free_entries_push(issue_table_ptr_t entry) {
	#pragma HLS inline
	#pragma HLS array_partition variable=free_entries  complete

	free_entries[free_entries_top] = entry;
	free_entries_top++;
	free_entries_empty = false;
	if (free_entries_bot == free_entries_top)
		free_entries_full = true;
}

void IssueStage::free_entries_push_and_pop(issue_table_ptr_t new_entry) {
	#pragma HLS inline
	#pragma HLS array_partition variable=free_entries  complete

	free_entries[free_entries_top] = new_entry;
	free_entries_bot++;
	free_entries_top++;
}

ap_uint<16> log2_16bits(const ap_uint<16>& in) {
	bool is_between_15_14 = in.test(15) || in.test(14);
	bool is_between_13_12 = in.test(13) || in.test(12);
	bool is_between_11_10 = in.test(11) || in.test(10);
	bool is_between_9_8   = in.test(9)  || in.test(8);
	bool is_between_7_6   = in.test(7)  || in.test(6);
	bool is_between_5_4   = in.test(5)  || in.test(4);
	bool is_between_3_2   = in.test(3)  || in.test(2);
	bool is_between_1_0   = in.test(1)  || in.test(0);
	ap_uint<16> between_15_14_res = in.test(15) ? 15 : 14;
	ap_uint<16> between_13_12_res = in.test(13) ? 13 : 12;
	ap_uint<16> between_11_10_res = in.test(11) ? 11 : 10;
	ap_uint<16> between_9_8_res   = in.test(9)  ? 9  : 8;
	ap_uint<16> between_7_6_res   = in.test(7)  ? 7  : 6;
	ap_uint<16> between_5_4_res   = in.test(5)  ? 5  : 4;
	ap_uint<16> between_3_2_res   = in.test(3)  ? 3  : 2;
	ap_uint<16> between_1_0_res   = in.test(1)  ? 1  : 0;

	bool is_between_15_12 = is_between_15_14 || is_between_13_12;
	bool is_between_11_8  = is_between_11_10 || is_between_9_8;
	bool is_between_7_4   = is_between_7_6   || is_between_5_4;
	bool is_between_3_0   = is_between_3_2   || is_between_1_0;
	ap_uint<16> between_15_12_res = is_between_15_14 ? between_15_14_res : between_13_12_res;
	ap_uint<16> between_11_8_res  = is_between_11_10 ? between_11_10_res : between_9_8_res;
	ap_uint<16> between_7_4_res   = is_between_7_6   ? between_7_6_res   : between_5_4_res;
	ap_uint<16> between_3_0_res   = is_between_3_2   ? between_3_2_res   : between_1_0_res;

	bool is_between_15_8 = is_between_15_12 || is_between_11_8;
	bool is_between_7_0  = is_between_7_4   || is_between_3_0;
	ap_uint<16> between_15_8_res = is_between_15_12 ? between_15_12_res : between_11_8_res;
	ap_uint<16> between_7_0_res  = is_between_7_4   ? between_7_4_res   : between_3_0_res;

	ap_uint<16> final_res = is_between_15_8 ? between_15_8_res : between_7_0_res;
	return final_res;
}

#ifdef DBG_SYNTH
void IssueStage::interface(const DecodeToIssue& from_decode, const WriteBackToIssue& from_write_back, const CommitToIssue& from_commit, IssueToWriteBack* to_write_back, IssueStatus* status) {
#else
void IssueStage::interface(const DecodeToIssue& from_decode, const WriteBackToIssue& from_write_back, const CommitToIssue& from_commit, IssueToWriteBack* to_write_back) {
#endif
	#pragma HLS inline
	#pragma HLS array_partition variable=register_file complete
	#pragma HLS array_partition variable=issue_table   complete
	#pragma HLS array_partition variable=free_entries  complete

	// TODO : update register file (the zero physical register must not be updated)

	// Selecting a ready instruction
	ap_uint<issue_table_entries_count> ready_entries_bitset = 0;
	for (uint16_t index = 0; index < issue_table_entries_count; index++) {
		#pragma HLS unroll
		IssueTableEntry entry = issue_table[index];
		// sources are ready if (use_src1 => src1_ready) && (use_src2 => src2_ready)
		bool sources_ready = (!entry.use_src1 || entry.src1_ready) && (!entry.use_src2 || entry.src2_ready);
		ready_entries_bitset.set_bit(index, entry.used && sources_ready);
	}
	issue_table_ptr_t ready_entry_index = log2_16bits(ready_entries_bitset);

	// Get a new entry index if a new one needs to be allocated
	issue_table_ptr_t new_entry_index = free_entries[free_entries_bot];

	bit_t is_entry_deallocated = !ready_entries_bitset.is_zero();
	bit_t is_entry_allocated   = from_decode.has_decoded_instruction;

	/* There are four possible operations
	* operation = 0b00 -> nothing to do
	* operation = 0b01 -> an entry is allocated: remove an index from the free entries' indices
	* operation = 0b10 -> an entry is deallocated: add its index to the free entries' indices
	* operation = 0b11 -> an entry is allocated, and another is deallocated
	*/
	ap_uint<2> operation = (is_entry_deallocated, is_entry_allocated);
	switch (operation)   {
	case 0b00:                                               break;
	case 0b01: free_entries_pop();                           break;
	case 0b10: free_entries_push(ready_entry_index);         break;
	case 0b11: free_entries_push_and_pop(ready_entry_index); break;
	}

	// Allocating the new instruction
	if (is_entry_allocated) {
		issue_table[new_entry_index] = (IssueTableEntry){
			true,
			from_decode.token,

			from_decode.pc,
			from_decode.category,
			from_decode.func3,
			from_decode.is_func7_0b0000000,
			from_decode.is_func7_0b0000001,
			from_decode.is_func7_0b0100000,
			from_decode.packed_immediate,

			from_decode.use_dest,
			from_decode.use_src1,
			from_decode.use_src2,
			from_decode.dest,
			from_decode.src1,
			from_decode.src2,
			from_decode.src1_ready,
			from_decode.src2_ready
			// ...
		};
	}

	// Deallocate the ready instruction and pass it through the compute units
	if (is_entry_deallocated) {
		issue_table[ready_entry_index].used = false;
		IssueTableEntry& entry = issue_table[ready_entry_index];

		ComputeUnitInput  compute_unit_input;
		ComputeUnitOutput compute_unit_output;

		compute_unit_input.pc                 = entry.pc;
		compute_unit_input.category           = entry.category;
		compute_unit_input.func3              = entry.func3;
		compute_unit_input.is_func7_0b0000000 = entry.is_func7_0b0000000;
		compute_unit_input.is_func7_0b0000001 = entry.is_func7_0b0000001;
		compute_unit_input.is_func7_0b0100000 = entry.is_func7_0b0100000;
		compute_unit_input.src1_value         = entry.use_src1 ? register_file[entry.src1] : static_cast<word_t>(0);
		compute_unit_input.src2_value         = entry.use_src2 ? register_file[entry.src2] : static_cast<word_t>(0);
		compute_unit_input.packed_immediate   = entry.packed_immediate;

		compute_unit.interface(compute_unit_input, &compute_unit_output);

		to_write_back->executed_instruction = true;
		to_write_back->token                = entry.token;
		to_write_back->has_result           = compute_unit_output.has_result;
		to_write_back->result               = compute_unit_output.result;
		to_write_back->dest                 = entry.dest;
		to_write_back->has_next_pc          = compute_unit_output.has_next_pc;
		to_write_back->next_pc              = compute_unit_output.next_pc;
		to_write_back->invalid              = compute_unit_output.invalid;
		to_write_back->is_load              = compute_unit_output.is_load;
		to_write_back->is_store             = compute_unit_output.is_store;
		to_write_back->is_byte_operation    = compute_unit_output.is_byte_operation;
		to_write_back->is_half_operation    = compute_unit_output.is_half_operation;
		to_write_back->is_word_operation    = compute_unit_output.is_word_operation;
		to_write_back->is_unsigned_extended = compute_unit_output.is_unsigned_extended;
		to_write_back->address              = compute_unit_output.address;
	}
	else {
		to_write_back->executed_instruction = false;
	}

	// Updating is done after having allocated the new instruction, because the sources' status could have changed
	// between the time an instruction is decoded and queued in the issue table
	if (from_write_back.has_a_src_ready) {
		register_file[from_write_back.src_ready] = from_write_back.result;
		for (uint16_t index = 0; index < issue_table_entries_count; index++) {
			#pragma HLS unroll
			issue_table[index].src1_ready |= issue_table[index].src1 == from_write_back.src_ready;
			issue_table[index].src2_ready |= issue_table[index].src2 == from_write_back.src_ready;
		}
	}

	// TODO : Block if there are no more free entries

	#ifndef __SYNTHESIS__
	json json_register_file = json::object();
	for (uint16_t i = 0; i < physical_register_count; i++) {
		json_register_file.push_back({ std::to_string(i), string_all(register_file[i].to_uint()) });
	}

	json json_issue_table = json::object();
	for (uint16_t i = 0; i < issue_table_entries_count; i++) {
		json_issue_table.push_back({ std::to_string(i), to_json(issue_table[i]) });
	}

	json json_free_entries = json::array();
	FOR_CYCLE_BUFFER(free_entries, issue_table_entries_count) {
		json_free_entries.push_back(free_entries[i].to_uint());
	}

	Debugger::add_cycle_event("Issue stage", {
		{ "Register file",      json_register_file            },
		{ "Issue table",        json_issue_table              },
		{ "Free entries",       json_free_entries             },
		{ "Free entries bot",   free_entries_bot.to_uint()    },
		{ "Free entries top",   free_entries_top.to_uint()    },
		{ "Free entries empty", free_entries_empty.to_bool()  },
		{ "Free entries full",  free_entries_full.to_bool()   },
		// Decisions
		{ "Entry allocated",   is_entry_allocated.to_bool()   },
		{ "Entry deallocated", is_entry_deallocated.to_bool() },
	});
	#endif // __SYNTHESIS__

	#ifdef DBG_SYNTH
	for (uint16_t i = 0; i < physical_register_count; i++)
		status->register_file[i] = register_file[i];
	for (uint16_t i = 0; i < issue_table_entries_count; i++)
		status->issue_table[i]   = issue_table[i];
	for (uint16_t i = 0; i < issue_table_entries_count; i++)
		status->free_entries[i]  = free_entries[i];
	status->free_entries_bot     = free_entries_bot;
	status->free_entries_top     = free_entries_top;
	status->free_entries_empty   = free_entries_empty;
	status->free_entries_full    = free_entries_full;
	#endif
}
