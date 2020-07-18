#include "processor.hpp"

#include "common.hpp"
#include "debug/debug.hpp"
#include "pipeline.hpp"

void processor(memory_t memory, bool* stop) {
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE s_axilite port=stop
	#pragma HLS INTERFACE ap_ctrl_none port=return

	Pipeline pipeline;
	int counter = 0;
	while (!(*stop)) {
		#ifndef __SYNTHESIS__
		Debugger::new_cycle();
		#endif // __SYNTHESIS__
		// pipeline.interface(memory, stop);
		if (counter == 10) {
			memory[0] = 0xFFCC8822;
			memory[1] = 0x00000001;
			*stop = true;
		}
		else {
			memory[counter] = 0x2266AAEE;
			counter++;
		}
	}
}