#include "stages/issue.hpp"

IssueStage::IssueStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active)
{
}

void IssueStage::interface(DecodeToIssue from_decode, IssueToDecode* to_decode, IssueToWriteBack* to_write_back) {
	if (is_active()) {
		// Insertion of the instruction from the decoding stage into the table
		if (table_free_list.is_empty()) {
			to_decode->block_decode_stage = true;
		}
		else {
			issue_table_ptr_t index = table_free_list.pop();
			table[index] = { true /* TODO : add additional informations about */ };
		}

		// Extraction of a ready instruction from the table, if there is one
		for (uint16_t index = 0; index < issue_table_entries_count; index++) {
			#pragma HLS UNROLL
			InstructionTableEntry& entry = table[index];
			bool sources_ready = /* TODO : use reg1 => reg1 ready && use reg2 => reg2 ready */ true;
			if (entry.used && sources_ready) {
				to_write_back->issue_instruction = true;
				// TODO : add all data necessary about the instruction
				entry.used = false;
				break; // TODO : ensure this happens only for one instruction, especially if several are ready, with the break
			}
			else {
				to_write_back->issue_instruction = false;
			}
		}
	}
	tick();
}