/* ****************************************************************************
*    Source file for the pipeline
**************************************************************************** */

#include "pipeline.hpp"

#include "common.hpp"
#include "debug/debug.hpp"

Pipeline::Pipeline() {
	IS_IN(decode_to_fetch).has_next_pc = true;
	IS_IN(decode_to_fetch).next_pc     = 0;

	IS_IN(fetch_to_decode).has_fetched = false;
	IS_IN(fetch_to_decode).instruction = 0;
	IS_IN(fetch_to_decode).pc          = 0;
}

#ifdef DBG_SYNTH
void Pipeline::interface(memory_t memory, large_bool* stop, DebugInfo* dbg) {
#else
void Pipeline::interface(memory_t memory, large_bool* stop) {
#endif // DBG_SYNTH
	#pragma HLS INLINE

	bit_t fetch_ran;
	bit_t decode_ran;

	fetch_stage. interface(memory, IS_IN(decode_to_fetch), &IS_OUT(fetch_to_decode), &fetch_ran);
	decode_stage.interface(IS_IN(fetch_to_decode), &IS_OUT(decode_to_fetch), stop, &decode_ran);

	// For C-simulation: transfer of the inter-stage structures from the
	// output of stages (in cycle i) to the input of stages (in cycle i + 1)
	TRANSFER_IS(fetch_to_decode);
	TRANSFER_IS(decode_to_fetch);

	#ifdef DBG_SYNTH
	dbg->fetch_to_decode = fetch_to_decode;
	dbg->decode_to_fetch = decode_to_fetch;
	dbg->fetch_ran  = fetch_ran;
	dbg->decode_ran = decode_ran;
	#endif // DBG_SYNTH
}