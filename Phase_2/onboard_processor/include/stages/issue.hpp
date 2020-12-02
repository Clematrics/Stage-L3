#pragma once

/* ****************************************************************************
*    Header file for the issue stage
**************************************************************************** */

#include "config.hpp"
#include "debug/debug.hpp"
#include "stage_structs.hpp"
#include "components/compute_unit.hpp"

class IssueStage {
	word_t            register_file[physical_register_count];

	IssueTableEntry   issue_table[issue_table_entries_count];

	issue_table_ptr_t free_entries[issue_table_entries_count];
	issue_table_ptr_t free_entries_bot;
	issue_table_ptr_t free_entries_top;
	bit_t             free_entries_empty;
	bit_t             free_entries_full;

	void free_entries_pop();
	void free_entries_push(issue_table_ptr_t entry);
	void free_entries_push_and_pop(issue_table_ptr_t new_entry);

	ComputeUnit compute_unit;
public:
	IssueStage();
	#ifdef DBG_SYNTH
	void interface(const DecodeToIssue& from_decode, const WriteBackToIssue& from_write_back, const CommitToIssue& from_commit, IssueToWriteBack* to_write_back, IssueStatus* status);
	#else
	void interface(const DecodeToIssue& from_decode, const WriteBackToIssue& from_write_back, const CommitToIssue& from_commit, IssueToWriteBack* to_write_back);
	#endif
};