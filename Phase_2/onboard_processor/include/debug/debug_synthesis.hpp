#pragma once

/* ****************************************************************************
*    Header file containing the definition of the debug structure for
*    synthesis
**************************************************************************** */

#include "common.hpp"
#include "config.hpp"
#include "stages/inter_stage.hpp"

#ifdef DBG_SYNTH
struct DebugInfo {
	word_t cycle;
	FetchToDecode fetch_to_decode;
	DecodeToFetch decode_to_fetch;
	bool fetch_locked;
	bool decode_locked;
};
#endif // DBG_SYNTH