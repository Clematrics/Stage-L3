#pragma once

/* ****************************************************************************
*    Header file defining all functions to convert stage structs to json
**************************************************************************** */

#ifndef __SYNTHESIS__

#include "architecture/architecture_helpers.hpp"
#include "debug/debug_helpers.hpp"
#include "debug/json.hpp"
#include "stages/stage_structs.hpp"


/* ****************************************************************************
*    For intra-stage structures
**************************************************************************** */

json to_json(const IssueTableEntry&    entry);
json to_json(const ReorderBufferEntry& entry);

/* ****************************************************************************
*    For inter-stage structures
**************************************************************************** */

json to_json(const FetchToDecode&     fetch_to_decode);
json to_json(const DecodeToFetch&     decode_to_fetch);
json to_json(const DecodeToIssue&     decode_to_issue);
json to_json(const DecodeToCommit&    decode_to_commit);
json to_json(const IssueToWriteBack&  issue_to_write_back);
json to_json(const WriteBackToFetch&  write_back_to_fetch);
json to_json(const WriteBackToIssue&  write_back_to_issue);
json to_json(const WriteBackToCommit& write_back_to_commit);
json to_json(const CommitToIssue&     commit_to_issue);
json to_json(const CommitToCommit&    commit_to_commit);

#endif // __SYNTHESIS__