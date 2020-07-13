#include "pipeline.hpp"

Pipeline::Pipeline()
	: fetch_stage     (5, 0),
	  decode_stage    (5, 1),
	  issue_stage     (5, 2),
	  write_back_stage(5, 3),
	  commit_stage    (5, 4)
{
	fetch_to_decode.has_fetched_instruction  = false;
	fetch_to_decode.program_counter          = 0;

	decode_to_issue.has_decoded_instruction  = false;

	decode_to_fetch.block_fetch_stage        = false;
	decode_to_fetch.next_program_counter     = false;

	decode_to_commit.has_decoded_instruction = false;

	// issue_to_decode      = {  };
	// issue_to_write_back  = {  };
	// write_back_to_commit = {  };
	// write_back_to_fetch  = {  };
	// write_back_to_issue  = {  };
}

void Pipeline::interface(memory_t memory, bool* stop) {
	#pragma HLS PIPELINE

	fetch_stage     .interface(memory, decode_to_fetch, write_back_to_fetch, &fetch_to_decode);
	decode_stage    .interface(fetch_to_decode, &decode_to_fetch, &decode_to_issue, &decode_to_commit, stop);
	issue_stage     .interface(decode_to_issue, write_back_to_issue, &issue_to_decode, &issue_to_write_back);
	write_back_stage.interface();
	commit_stage    .interface();
}