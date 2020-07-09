#include "pipeline.hpp"

Pipeline::Pipeline()
	: fetch_stage     (5, 0),
	  decode_stage    (5, 1),
	  issue_stage     (5, 2),
	  write_back_stage(5, 3),
	  commit_stage    (5, 4),
	  fetch_to_decode      { false, 0, 0 },
	  decode_to_issue      { false },
	  decode_to_fetch      { false, 0 },
	  decode_to_commit     { false },
	  issue_to_decode      {  },
	  issue_to_write_back  {  },
	  write_back_to_commit {  },
	  write_back_to_fetch  {  },
	  write_back_to_issue  {  }
{
}

void Pipeline::interface(memory_t memory, bool* stop) {
	#pragma HLS PIPELINE

	fetch_stage     .interface(memory, decode_to_fetch, write_back_to_fetch, &fetch_to_decode);
	decode_stage    .interface(fetch_to_decode, &decode_to_fetch, &decode_to_issue, stop);
	issue_stage     .interface(decode_to_issue, &issue_to_decode, &issue_to_write_back);
	write_back_stage.interface();
	commit_stage    .interface();
}