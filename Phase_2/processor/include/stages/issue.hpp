#pragma once

#include "stage.hpp"
#include "inter_stage.hpp"
#include "components/circular_buffer.hpp"

struct InstructionTableEntry {
	bool used;
	// TODO : information about instruction
	physical_reg_t src1;
	physical_reg_t src2;
	physical_reg_t dest;
	bool use_src1;
	bool use_src2;
	bool src1_ready;
	bool src2_ready;
};

class IssueStage : Stage {
	physical_reg_t register_file[physical_register_count];
	InstructionTableEntry table[issue_table_entries_count];
	CircularBuffer<issue_table_ptr_t, issue_table_entries_count> table_free_list;
public:
	IssueStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface(DecodeToIssue from_decode, WriteBackToIssue write_back_to_issue, IssueToDecode* to_decode, IssueToWriteBack* to_write_back);
};