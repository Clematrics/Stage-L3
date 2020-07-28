#pragma once

/* ****************************************************************************
*    Header file containing the definition of the debug structure for
*    synthesis
**************************************************************************** */

#include "common.hpp"
#include "config.hpp"
#include "stages/stage_structs.hpp"

#ifdef DBG_SYNTH
struct FetchStatus {
	program_counter_t program_counter;

	bit_t did_smth;
};

struct DecodeStatus {
	physical_reg_t register_map[architectural_register_count];
	ap_uint<physical_register_count> ready_registers;

	physical_reg_t free_aliases[physical_register_count];
	physical_reg_ptr_t free_aliases_bot;
	physical_reg_ptr_t free_aliases_top;
	bit_t free_aliases_empty;
	bit_t free_aliases_full;

	bit_t did_smth;
};

struct IssueStatus {
	word_t register_file[physical_register_count];
	IssueTableEntry issue_table[issue_table_entries_count];
	issue_table_ptr_t free_entries[issue_table_entries_count];
	issue_table_ptr_t free_entries_bot;
	issue_table_ptr_t free_entries_top;
	bit_t free_entries_empty;
	bit_t free_entries_full;
};

struct WriteBackStatus {

};

struct CommitStatus {
	ReorderBufferEntry reorder_buffer[reorder_buffer_count];
	reorder_buffer_ptr_t reorder_buffer_bot;
	reorder_buffer_ptr_t reorder_buffer_top;
	bit_t                reorder_buffer_empty;
	bit_t                reorder_buffer_full;
};


struct DebugInfo {
	word_t cycle;

	FetchStatus     fetch_status;
	DecodeStatus    decode_status;
	IssueStatus     issue_status;
	WriteBackStatus write_back_status;
	CommitStatus    commit_status;

	FetchToDecode fetch_to_decode;
	DecodeToFetch decode_to_fetch;
	DecodeToIssue decode_to_issue;
	DecodeToCommit decode_to_commit;
	CommitToCommit commit_to_commit;
};
#endif // DBG_SYNTH