/* ****************************************************************************
*    Source file for the pipeline
**************************************************************************** */

#include "pipeline.hpp"

#include "common.hpp"
#include "debug/debug.hpp"

Pipeline::Pipeline() {
	IS_IN(fetch_to_decode).has_fetched = false;
	IS_IN(fetch_to_decode).instruction = 0;
	IS_IN(fetch_to_decode).pc          = 0;

	IS_IN(decode_to_fetch).has_next_pc = true;
	IS_IN(decode_to_fetch).next_pc     = 0;

	IS_IN(decode_to_issue).has_decoded_instruction = false;

	IS_IN(commit_to_commit).previous_first_entry.token   = initial_token;
	IS_IN(commit_to_commit).previous_first_entry.done    = false;
	IS_IN(commit_to_commit).previous_first_entry.invalid = false;
}

#ifdef DBG_SYNTH
void Pipeline::interface(memory_t memory, bit_t* stop, DebugInfo* dbg) {
#else
void Pipeline::interface(memory_t memory, bit_t* stop) {
#endif // DBG_SYNTH
	#pragma HLS inline

	bit_t fetch_ran;
	bit_t decode_ran;
	bit_t issue_ran;
	bit_t commit_ran;

	#ifdef DBG_SYNTH
	#define DBG_FETCH_STATUS      , &dbg->fetch_status
	#define DBG_DECODE_STATUS     , &dbg->decode_status
	#define DBG_ISSUE_STATUS      , &dbg->issue_status
	#define DBG_WRITE_BACK_STATUS , &dbg->write_back_status
	#define DBG_COMMIT_STATUS     , &dbg->commit_status
	#else
	#define DBG_FETCH_STATUS
	#define DBG_DECODE_STATUS
	#define DBG_ISSUE_STATUS
	#define DBG_WRITE_BACK_STATUS
	#define DBG_COMMIT_STATUS
	#endif

	fetch_stage. interface(memory, IS_IN(decode_to_fetch), &IS_OUT(fetch_to_decode) DBG_FETCH_STATUS);
	decode_stage.interface(IS_IN(fetch_to_decode), &IS_OUT(decode_to_fetch), &IS_OUT(decode_to_issue), &IS_OUT(decode_to_commit) DBG_DECODE_STATUS);
	issue_stage. interface(IS_IN(decode_to_issue), IS_IN(write_back_to_issue), IS_IN(commit_to_issue), &IS_OUT(issue_to_write_back) DBG_ISSUE_STATUS);
	commit_stage.interface(IS_IN(decode_to_commit), IS_IN(commit_to_commit), &IS_OUT(commit_to_commit), stop DBG_COMMIT_STATUS);

	// For C-simulation: transfer of the inter-stage structures from the
	// output of stages (in cycle i) to the input of stages (in cycle i + 1)
	TRANSFER_IS(fetch_to_decode);
	TRANSFER_IS(decode_to_fetch);
	TRANSFER_IS(decode_to_issue);
	TRANSFER_IS(decode_to_commit);
	TRANSFER_IS(issue_to_write_back);
	TRANSFER_IS(write_back_to_issue);
	TRANSFER_IS(commit_to_issue);
	TRANSFER_IS(commit_to_commit);

	#ifdef DBG_SYNTH
	dbg->fetch_to_decode  = IS_OUT(fetch_to_decode);
	dbg->decode_to_fetch  = IS_OUT(decode_to_fetch);
	dbg->decode_to_issue  = IS_OUT(decode_to_issue);
	dbg->decode_to_commit = IS_OUT(decode_to_commit);
	#endif // DBG_SYNTH
}