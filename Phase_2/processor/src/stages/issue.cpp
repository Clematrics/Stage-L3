#include "stages/issue.hpp"

IssueStage::IssueStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active)
{
}

word_t log2_16bits_standford(const word_t& in) {
	// From https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
	word_t value = in;
	const word_t b[] = {0x2, 0xC, 0xF0, 0xFF00};
	const word_t S[] = {1, 2, 4, 8};
	word_t r = 0; // result of log2(value)

	for (int8_t i = 3; i >= 0; i--) {
		#pragma HLS UNROLL
		if (value & b[i]) {
			value >>= S[i];
			r |= S[i];
		}
	}
	return r;
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

void IssueStage::interface(const DecodeToIssue& from_decode, const WriteBackToIssue& from_write_back, IssueToDecode* to_decode, IssueToWriteBack* to_write_back) {
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

			to_decode->block_decode_stage = table_free_list.is_empty();
		}

		// updating ready instructions
		if (from_write_back.has_a_src_ready) {
			for (uint16_t index = 0; index < issue_table_entries_count; index++) {
				#pragma HLS UNROLL
				table[index].src1_ready = table[index].src1 == from_write_back.src_ready;
				table[index].src2_ready = table[index].src2 == from_write_back.src_ready;
			}

			register_file[from_write_back.src_ready] = from_write_back.result;
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
			to_write_back->issued_instruction = false;
		}
		else {
			to_write_back->issued_instruction = true;
			issue_table_ptr_t selected_index = log2_16bits(ready_entries_bitset);

			table[selected_index].used = false;
		}

		// Computing
		AluInput  alu_input;
		AluOutput alu_output;
		BranchUnitInput  branch_unit_input;
		BranchUnitOutput branch_unit_output;
		MemoryUnitInput  memory_unit_input;
		MemoryUnitOutput memory_unit_output;

		alu_input.func3            = branch_unit_input.func3            = memory_unit_input.func3            = from_decode.func3;
		alu_input.src1_value       = branch_unit_input.src1_value       = memory_unit_input.src1_value       = from_decode.src1;
		alu_input.src2_value       = branch_unit_input.src2_value       = memory_unit_input.src2_value       = from_decode.src2;
		alu_input.packed_immediate = branch_unit_input.packed_immediate = memory_unit_input.packed_immediate = from_decode.packed_immediate;
		alu_input.current_pc       = branch_unit_input.current_pc                                            = from_decode.program_counter;

		alu_input.kind = from_decode.kind;
		alu_input.is_func7_0b0000000 = from_decode.is_func7_0b0000000;
		alu_input.is_func7_0b0100000 = from_decode.is_func7_0b0100000;
		alu_input.is_func7_0b0000001 = from_decode.is_func7_0b0000001;

		alu.interface(alu_input, &alu_output);
		branch_unit.interface(branch_unit_input, &branch_unit_output);
		memory_unit.interface(memory_unit_input, &memory_unit_output);

		// Selecting the good result and forwarding informations to the write back stage
		to_write_back->has_next_pc = false;
		to_write_back->is_memory_instruction = false;
		if (alu_output.valid_instruction) {
			to_write_back->result = alu_output.result;
			to_write_back->destination = from_decode.dest;
			to_write_back->has_next_pc = alu_output.has_next_pc;
		}
		else if (branch_unit_output.valid_instruction) {
			to_write_back->destination = Architecture::Register::zero;
			to_write_back->has_next_pc = true;
			to_write_back->next_program_counter = branch_unit_output.next_pc;
		}
		else if (memory_unit_output.valid_instruction) {
			to_write_back->is_memory_instruction = true;
			to_write_back->result = memory_unit_output.effective_address;
			to_write_back->operation = memory_unit_output.operation;
			to_write_back->size = memory_unit_output.size;
			to_write_back->sign = memory_unit_output.sign;
		}
	}
	tick();
}