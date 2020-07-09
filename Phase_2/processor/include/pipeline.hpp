#pragma once

#include "stages/fetch.hpp"
#include "stages/decode.hpp"
#include "stages/issue.hpp"
#include "stages/write_back.hpp"
#include "stages/commit.hpp"

#include "stages/inter_stage.hpp"

class Pipeline {
	// Stages
	FetchStage     fetch_stage;
	DecodeStage    decode_stage;
	IssueStage     issue_stage;
	WriteBackStage write_back_stage;
	CommitStage    commit_stage;

	// Inter-stage communication structures
	FetchToDecode     fetch_to_decode;
	DecodeToIssue     decode_to_issue;
	DecodeToFetch     decode_to_fetch;
	DecodeToCommit    decode_to_commit;
	IssueToDecode     issue_to_decode;
	IssueToWriteBack  issue_to_write_back;
	WriteBackToCommit write_back_to_commit;
	WriteBackToFetch  write_back_to_fetch;
	WriteBackToIssue  write_back_to_issue;
public:
	Pipeline();
	void interface(memory_t memory, bool* stop);
};