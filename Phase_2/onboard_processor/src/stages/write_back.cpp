/* ****************************************************************************
*    Source file for the write back stage
**************************************************************************** */

#include "stages/write_back.hpp"

#include "debug/debug.hpp"

WriteBackStage::WriteBackStage() {
}

#ifdef DBG_SYNTH
void WriteBackStage::interface(const IssueToWriteBack& from_issue, WriteBackToFetch* to_fetch, WriteBackToIssue* to_issue, WriteBackToCommit* to_commit, WriteBackStatus* status) {
#else
void WriteBackStage::interface(const IssueToWriteBack& from_issue, WriteBackToFetch* to_fetch, WriteBackToIssue* to_issue, WriteBackToCommit* to_commit) {
#endif
	#pragma HLS inline

	to_issue->has_a_src_ready = false;

	to_commit->executed_instruction = from_issue.executed_instruction;
	to_commit->token                = from_issue.token;
	to_commit->invalid              = from_issue.invalid;
	to_commit->is_load              = from_issue.is_load;
	to_commit->is_store             = from_issue.is_store;
	to_commit->is_byte_operation    = from_issue.is_byte_operation;
	to_commit->is_half_operation    = from_issue.is_half_operation;
	to_commit->is_word_operation    = from_issue.is_word_operation;
	to_commit->is_unsigned_extended = from_issue.is_unsigned_extended;
	to_commit->load_dest            = from_issue.dest;
	to_commit->value                = from_issue.result;
	to_commit->address              = from_issue.address;

	if (from_issue.executed_instruction) {
		if (from_issue.has_result && !from_issue.invalid) {
			to_issue->has_a_src_ready = true;
			to_issue->src_ready       = from_issue.dest;
			to_issue->result          = from_issue.result;
		}

		if (from_issue.has_next_pc && !from_issue.invalid) {
			to_fetch->has_next_pc = true;
			to_fetch->next_pc     = from_issue.next_pc;
		}
	}

	#ifndef __SYNTHESIS__
	Debugger::add_cycle_event("Write back stage", {
		// no internal state
	});
	#endif // __SYNTHESIS__
}