/* ****************************************************************************
*    Source file for the top function
**************************************************************************** */

#include "processor.hpp"

#include "common.hpp"
#include "debug/debug.hpp"
#include "pipeline.hpp"

#ifdef DBG_SYNTH
void processor(memory_t memory, TW(large_bool) run, bit_t* stopped, DebugInfo* dbg) {
	#pragma HLS INTERFACE s_axilite port=run
	#pragma HLS INTERFACE s_axilite port=dbg
#else
void processor(memory_t memory, bit_t* stopped) {
#endif
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE s_axilite port=stopped
	#pragma HLS INTERFACE ap_ctrl_none port=return

	Pipeline pipeline;
	word_t cycle = 0;
	bit_t stop = false;
	bit_t stop_pipeline = false;

	while (!stop) {
		#pragma HLS PIPELINE
		// #pragma HLS DEPENDENCE variable=stop inter false
		// #pragma HLS DEPENDENCE variable=stop intra false
		stop = stop_pipeline;

		#ifdef DBG_SYNTH
		if (*run) { // Waiting for the 'run' signal to be true to do a cycle
		#endif
			#ifndef __SYNTHESIS__
			Debugger::new_cycle();
			#endif // __SYNTHESIS__

			#ifdef DBG_SYNTH
			pipeline.interface(memory, &stop_pipeline, dbg);
			dbg->cycle = cycle;
			cycle++;
			#else
			pipeline.interface(memory, &stop_pipeline);
			#endif

		#ifdef DBG_SYNTH
			*run = false; // Cycle has ended, 'run' is set to false
		}
		#endif
	}
	*stopped = true;
}