/* ****************************************************************************
*    Source file for the top function
**************************************************************************** */

#include "processor.hpp"

#include "common.hpp"
#include "debug/debug.hpp"
#include "pipeline.hpp"

#ifdef DBG_SYNTH
void processor(memory_t memory, TW(large_bool) run, TW(large_bool) stop, DebugInfo* dbg) {
	#pragma HLS INTERFACE s_axilite port=run
	#pragma HLS INTERFACE s_axilite port=dbg
#else
void processor(memory_t memory, TW(large_bool) stop) {
#endif
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE s_axilite port=stop
	#pragma HLS INTERFACE ap_ctrl_none port=return

	Pipeline pipeline;
	word_t cycle = 0;

	while (! *stop) {
		#pragma HLS PIPELINE
		#pragma HLS DEPENDENCE variable=stop intra false

		#ifdef DBG_SYNTH
		if (*run) { // Waiting for the 'run' signal to be true to do a cycle
		#endif
			#ifndef __SYNTHESIS__
			Debugger::new_cycle();
			#endif // __SYNTHESIS__

			#ifdef DBG_SYNTH
			pipeline.interface(memory, stop, dbg);
			dbg->cycle = cycle;
			cycle++;
			#else
			pipeline.interface(memory, stop);
			#endif

		#ifdef DBG_SYNTH
			*run = false; // Cycle has ended, 'run' is set to false
		}
		#endif
	}
}