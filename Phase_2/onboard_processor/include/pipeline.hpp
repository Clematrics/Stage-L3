#pragma once

/* ****************************************************************************
*    Header file for the pipeline
**************************************************************************** */

#include "config.hpp"
#include "common.hpp"
#include "debug/debug.hpp"
#include "stages/fetch.hpp"
#include "stages/decode.hpp"
#include "stages/issue.hpp"
#include "stages/commit.hpp"
#include "stages/stage_structs.hpp"

/* ****************************************************************************
*    Macros to simulate the behavior of the pipeline on hardware
*    IS stands for Inter Stage
**************************************************************************** */
#ifndef __SYNTHESIS__
#define IS_IN(name)  in_##name
#define IS_OUT(name) out_##name
#define DECL_IS(type, name) type IS_IN(name); type IS_OUT(name)
#define TRANSFER_IS(name) IS_IN(name) = IS_OUT(name)
#else
#define IS_IN(name)  in_##name
#define IS_OUT(name) out_##name
#define DECL_IS(type, name) type IS_IN(name); type IS_OUT(name)
#define TRANSFER_IS(name) IS_IN(name) = IS_OUT(name)
#endif

class Pipeline {
	// Stages
	FetchStage  fetch_stage;
	DecodeStage decode_stage;
	IssueStage  issue_stage;
	CommitStage commit_stage;

	// Inter stage structures
	DECL_IS(FetchToDecode,  fetch_to_decode);
	DECL_IS(DecodeToFetch,  decode_to_fetch);
	DECL_IS(DecodeToIssue,  decode_to_issue);
	DECL_IS(DecodeToCommit, decode_to_commit);
	DECL_IS(CommitToCommit, commit_to_commit);
public:
	Pipeline();

	#ifdef DBG_SYNTH
	void interface(memory_t memory, bit_t* stop, DebugInfo* dbg);
	#else
	void interface(memory_t memory, bit_t* stop);
	#endif
};