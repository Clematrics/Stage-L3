#include "processor.hpp"

#include "common.hpp"
#include "debug/debug.hpp"
#include "pipeline.hpp"

void processor(memory_t memory, bool* stop) {
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE s_axilite port=stop
	#pragma HLS INTERFACE ap_ctrl_none port=return

	Pipeline pipeline;
	while (!(*stop)) {
		#ifndef __SYNTHESIS__
		Debugger::new_cycle();
		#endif // __SYNTHESIS__
		pipeline.interface(memory, stop);
	}
}