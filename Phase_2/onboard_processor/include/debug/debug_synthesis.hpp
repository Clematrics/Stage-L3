#pragma once

/* ****************************************************************************
*    Header file containing the definition of the debug structure for
*    synthesis
**************************************************************************** */

#include "common.hpp"
#include "config.hpp"
#include "stages/stage_structs.hpp"

#ifdef DBG_SYNTH
struct DebugInfo {
	word_t cycle;
	FetchToDecode fetch_to_decode;
	DecodeToFetch decode_to_fetch;
	DecodeToIssue decode_to_issue;
	DecodeToCommit decode_to_commit;
	CommitToCommit commit_to_commit;
	bit_t fetch_ran;
	bit_t decode_ran;
};
#endif // DBG_SYNTH