/* ****************************************************************************
*    Source file for the issue stage
**************************************************************************** */

#include "stages/issue.hpp"
#include "stages/stage_structs.hpp"

#include "debug/debug.hpp"

IssueStage::IssueStage() {
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

void IssueStage::free_entries_push_back(issue_table_ptr_t entry) {
	#pragma HLS inline
	free_entries[free_entries_top] = entry;
	free_entries_top++;
	free_entries_empty = false;
	if (free_entries_bot == free_entries_top)
		free_entries_full = true;
}

void IssueStage::free_entries_pop() {
	#pragma HLS inline
	free_entries_bot++;
	free_entries_full = false;
	if (free_entries_bot == free_entries_top)
		free_entries_empty = true;
}

void IssueStage::free_entries_push_and_pop(issue_table_ptr_t new_entry) {
	#pragma HLS inline
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

void IssueStage::interface(DecodeToIssue from_decode, WriteBackToIssue from_write_back, CommitToIssue from_commit, IssueToWriteBack* to_write_back, bit_t* issue_ran) {
	#pragma HLS inline
	#pragma HLS array_partition variable=register_file complete
	#pragma HLS array_partition variable=issue_table   complete
	#pragma HLS array_partition variable=free_entries  complete

	// TODO : update ready instructions if a new one is ready
	if (from_write_back.has_a_src_ready) {
		for (uint16_t index = 0; index < issue_table_entries_count; index++) {
			#pragma HLS unroll
			issue_table[index].src1_ready = issue_table[index].src1 == from_write_back.src_ready;
			issue_table[index].src2_ready = issue_table[index].src2 == from_write_back.src_ready;
		}
	}

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
	switch (is_entry_allocated)   {
	case 0b00:                                               break;
	case 0b01: free_entries_pop();                           break;
	case 0b10: free_entries_push_back(ready_entry_index);    break;
	case 0b11: free_entries_push_and_pop(ready_entry_index); break;
	}

	// Allocating the new instruction
	if (is_entry_allocated) {
		issue_table[new_entry_index] = (IssueTableEntry){
			true,
			from_decode.use_src1,
			from_decode.use_src2,
			true, // TODO : retrieve the status of src_1 to check if it is already ready
			true, // TODO : retrieve the status of src_2 to check if it is already ready
			from_decode.src1,
			from_decode.src2,
			// ...
		};
	}

	// Deallocate the ready instruction and pass it through the compute units
	if (is_entry_deallocated) {
		issue_table[ready_entry_index].used = false;
	}

	// TODO : Block if there are no more free entries

	// TODO : make compute units

	// TODO : send results to write back

	#ifndef __SYNTHESIS__
	Debugger::add_cycle_event({
		{ "Issue stage",
			{
				{ "Entry allocated",   is_entry_allocated   },
				{ "Entry deallocated", is_entry_deallocated }
			}
		}
	});
	#endif // __SYNTHESIS__
}
