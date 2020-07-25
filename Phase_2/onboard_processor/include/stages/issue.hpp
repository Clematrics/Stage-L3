#pragma once

/* ****************************************************************************
*    Header file for the issue stage
**************************************************************************** */

#include "stage_structs.hpp"

class IssueStage {
	word_t register_file[physical_register_count];

	IssueTableEntry issue_table[issue_table_entries_count];
	issue_table_ptr_t free_entries[issue_table_entries_count];
	issue_table_ptr_t free_entries_bot;
	issue_table_ptr_t free_entries_top;
	bit_t free_entries_empty;
	bit_t free_entries_full;

	void free_entries_push_back(issue_table_ptr_t entry);
	void free_entries_pop();
	void free_entries_push_and_pop(issue_table_ptr_t new_entry);
public:
	IssueStage();
	void interface(DecodeToIssue from_decode, WriteBackToIssue from_write_back, CommitToIssue from_commit, IssueToWriteBack* to_write_back, bit_t* issue_ran);
};