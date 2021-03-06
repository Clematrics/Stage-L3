#include "../include/issue.hpp"

Issue::Issue()
{
	for (uint16_t i = 0; i < issue_table_entries_count; i++) {
		#pragma HLS UNROLL
		free_list.push_back(i);
	}
}

void Issue::issue(DecodedInstruction instruction_in, bool* blocking, bool* instruction_ready, DecodedInstruction* instruction_out) {
	// Insertion of the instruction from the decoding stage into the table
	if (free_list.is_empty()) {
		*blocking = true;
	}
	else {
		issue_table_ptr_t index = free_list.pop();
		table[index] = { instruction_in, true };
	}

	// Extraction of a ready instruction from the table, if there is one
	for (uint16_t index = 0; index < issue_table_entries_count; index++) {
		#pragma HLS UNROLL
		InstructionTableEntry& entry = table[index];
		bool sources_ready = /* TODO : use reg1 => reg1 ready && use reg2 => reg2 ready */ true;
		if (entry.used && sources_ready) {
			*instruction_ready = true;
			*instruction_out = entry.instr;
			entry.used = false;
			break; // TODO : ensure this happens only for one instruction, especially if several are ready, with the break
		}
		else {
			*instruction_ready = false;
		}
	}
}