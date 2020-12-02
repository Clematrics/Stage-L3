/* ****************************************************************************
*    Source file for the pipeline
**************************************************************************** */

#include "pipeline.hpp"

#include "common.hpp"
#include "debug/debug.hpp"

Pipeline::Pipeline() {
	IS_IN(fetch_to_decode).has_fetched = false;

	IS_IN(decode_to_fetch).has_next_pc = true;
	IS_IN(decode_to_fetch).next_pc     = 0;

	IS_IN(decode_to_issue).has_decoded_instruction = false;

	IS_IN(decode_to_commit).add_to_rob = false;

	IS_IN(issue_to_write_back).has_next_pc          = false;
	IS_IN(issue_to_write_back).has_result           = false;
	IS_IN(issue_to_write_back).executed_instruction = false;

	IS_IN(write_back_to_fetch).has_next_pc           = false;
	IS_IN(write_back_to_issue).has_a_src_ready       = false;
	IS_IN(write_back_to_commit).executed_instruction = false;

	IS_IN(commit_to_issue).has_loaded_value = false;

	IS_IN(commit_to_commit).rob_was_empty = false;
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

	fetch_stage.     interface(memory, IS_IN(decode_to_fetch), IS_IN(write_back_to_fetch), &IS_OUT(fetch_to_decode) DBG_FETCH_STATUS);
	decode_stage.    interface(IS_IN(fetch_to_decode), &IS_OUT(decode_to_fetch), &IS_OUT(decode_to_issue), &IS_OUT(decode_to_commit) DBG_DECODE_STATUS);
	issue_stage.     interface(IS_IN(decode_to_issue), IS_IN(write_back_to_issue), IS_IN(commit_to_issue), &IS_OUT(issue_to_write_back) DBG_ISSUE_STATUS);
	write_back_stage.interface(IS_IN(issue_to_write_back), &IS_OUT(write_back_to_fetch), &IS_OUT(write_back_to_issue), &IS_OUT(write_back_to_commit) DBG_WRITE_BACK_STATUS);
	commit_stage.    interface(memory, IS_IN(decode_to_commit), IS_IN(write_back_to_commit), IS_IN(commit_to_commit), &IS_OUT(commit_to_issue), &IS_OUT(commit_to_commit), stop DBG_COMMIT_STATUS);

	// For C-simulation: transfer of the inter-stage structures from the
	// output of stages (in cycle i) to the input of stages (in cycle i + 1)
	TRANSFER_IS(fetch_to_decode);
	TRANSFER_IS(decode_to_fetch);
	TRANSFER_IS(decode_to_issue);
	TRANSFER_IS(decode_to_commit);
	TRANSFER_IS(issue_to_write_back);
	TRANSFER_IS(write_back_to_fetch);
	TRANSFER_IS(write_back_to_issue);
	TRANSFER_IS(write_back_to_commit);
	TRANSFER_IS(commit_to_issue);
	TRANSFER_IS(commit_to_commit);

	#ifndef __SYNTHESIS__
	Debugger::add_cycle_event("Fetch to decode",      to_json(IS_IN(fetch_to_decode)));
	Debugger::add_cycle_event("Decode to fetch",      to_json(IS_IN(decode_to_fetch)));
	Debugger::add_cycle_event("Decode to issue",      to_json(IS_IN(decode_to_issue)));
	Debugger::add_cycle_event("Decode to commit",     to_json(IS_IN(decode_to_commit)));
	Debugger::add_cycle_event("Issue to write back",  to_json(IS_IN(issue_to_write_back)));
	Debugger::add_cycle_event("Write back to fetch",  to_json(IS_IN(write_back_to_fetch)));
	Debugger::add_cycle_event("Write back to issue",  to_json(IS_IN(write_back_to_issue)));
	Debugger::add_cycle_event("Write back to commit", to_json(IS_IN(write_back_to_commit)));
	Debugger::add_cycle_event("Commit to issue",      to_json(IS_IN(commit_to_issue)));
	Debugger::add_cycle_event("Commit to commit",     to_json(IS_IN(commit_to_commit)));
	#endif

	#ifdef DBG_SYNTH
	dbg->fetch_to_decode      = IS_OUT(fetch_to_decode);
	dbg->decode_to_fetch      = IS_OUT(decode_to_fetch);
	dbg->decode_to_issue      = IS_OUT(decode_to_issue);
	dbg->decode_to_commit     = IS_OUT(decode_to_commit);
	dbg->issue_to_write_back  = IS_OUT(issue_to_write_back);
	dbg->write_back_to_fetch  = IS_OUT(write_back_to_fetch);
	dbg->write_back_to_issue  = IS_OUT(write_back_to_issue);
	dbg->write_back_to_commit = IS_OUT(write_back_to_commit);
	dbg->commit_to_issue      = IS_OUT(commit_to_issue);
	dbg->commit_to_commit     = IS_OUT(commit_to_commit);
	#endif // DBG_SYNTH
}

/*
WARNING: [SCHED 204-68] The II Violation in module 'processor' (Loop: Loop 4): Unable to enforce a carried dependence constraint (II = 1, distance = 1, offset = 0)
   between 'store' operation
   	(	'pipeline_in_fetch_to_decode_pc_V_write_ln62',
		onboard_processor/src/stages/commit.cpp:62
	->	onboard_processor/src/pipeline.cpp:55
	->	onboard_processor/src/processor.cpp:42
	) of variable 'pipeline.in_write_back_to_fetch.next_pc.V',
		onboard_processor/src/stages/write_back.cpp:41
	->	onboard_processor/src/pipeline.cpp:54
	->	onboard_processor/src/processor.cpp:42
	on local variable 'pipeline.in_fetch_to_decode.pc.V'
	and 'load' operation
	(	'pipeline_in_fetch_to_decode_pc_V_load_1',
		onboard_processor/src/stages/fetch.cpp:22
	->	onboard_processor/src/pipeline.cpp:51
	->	onboard_processor/src/processor.cpp:42
	) on local variable 'pipeline.in_fetch_to_decode.pc.V'.
*/