#include "stages/issue.hpp"

IssueStage::IssueStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active)
{
}

word_t log2_32bits(const word_t& in) {
	// From https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
	word_t value = in;
	const word_t b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
	const word_t S[] = {1, 2, 4, 8, 16};
	word_t r = 0; // result of log2(value)

	for (int8_t i = 4; i >= 0; i--) {
		#pragma HLS UNROLL
		if (value & b[i]) {
			value >>= S[i];
			r |= S[i];
		}
	}
	return r;
}

void IssueStage::interface(DecodeToIssue from_decode, WriteBackToIssue write_back_to_issue, IssueToDecode* to_decode, IssueToWriteBack* to_write_back) {
	#pragma HLS INLINE

	if (is_active()) {
		// Insertion of the instruction from the decoding stage into the table
		if (table_free_list.is_empty()) {
			to_decode->block_decode_stage = true;
		}
		else {
			issue_table_ptr_t index = table_free_list.pop();
			table[index].used       = true;
			table[index].src1       = from_decode.src1;
			table[index].src2       = from_decode.src2;
			table[index].dest       = from_decode.dest;
			table[index].use_src1   = from_decode.use_src1;
			table[index].use_src2   = from_decode.use_src2;
			table[index].src1_ready = true; // TODO : retrieve the status of src_1 to check if it is already ready
			table[index].src2_ready = true; // TODO : retrieve the status of src_2 to check if it is already ready
			/* TODO : add additional informations about */
		}

		// updating ready instructions
		if (write_back_to_issue.has_a_src_ready) {
			for (uint16_t index = 0; index < issue_table_entries_count; index++) {
				#pragma HLS UNROLL
				table[index].src1_ready = table[index].src1 == write_back_to_issue.src_ready;
				table[index].src2_ready = table[index].src2 == write_back_to_issue.src_ready;
			}
		}

		// Extraction of a ready instruction from the table, if there is one
		ap_uint<issue_table_entries_count> ready_entries_bitset = 0;
		for (uint16_t index = 0; index < issue_table_entries_count; index++) {
			#pragma HLS UNROLL
			InstructionTableEntry entry = table[index];
			// sources are ready if (use_src1 => src1_ready) && (use_src2 => src2_ready)
			bool sources_ready = (!entry.use_src1 || entry.src1_ready) && (!entry.use_src2 || entry.src2_ready);
			if (entry.used && sources_ready) {
				ready_entries_bitset.set(index);
			}
		}

		if (ready_entries_bitset.is_zero()) {
			to_write_back->issue_instruction = false;
		}
		else {
			to_write_back->issue_instruction = true;
			issue_table_ptr_t selected_index = log2_32bits(ready_entries_bitset);

			table[selected_index].used = false;
			// TODO : add all data necessary about the instruction in to_write_back
		}

	}
	tick();
}