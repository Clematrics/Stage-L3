#pragma once

/* ****************************************************************************
*    Header file for the issue stage
**************************************************************************** */

#include "inter_stage.hpp"

class IssueStage {
	program_counter_t program_counter;
public:
	IssueStage();
	void interface(DecodeToIssue from_decode, WriteBackToIssue from_write_back, CommitToIssue from_commit, IssueToWriteBack* to_write_back, bit_t* issue_ran);
};