#pragma once

/* ****************************************************************************
*    Header file for the write back stage
**************************************************************************** */

#include "config.hpp"
#include "common.hpp"
#include "debug/debug.hpp"
#include "stage_structs.hpp"

class WriteBackStage {
public:
	WriteBackStage();
	#ifdef DBG_SYNTH
	void interface(const IssueToWriteBack& from_issue, WriteBackToFetch* to_fetch, WriteBackToIssue* to_issue, WriteBackToCommit* to_commit, WriteBackStatus* status);
	#else
	void interface(const IssueToWriteBack& from_issue, WriteBackToFetch* to_fetch, WriteBackToIssue* to_issue, WriteBackToCommit* to_commit);
	#endif
};