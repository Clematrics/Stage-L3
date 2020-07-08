#include "processor.hpp"

#include "common.hpp"
#include "pipeline.hpp"

void processor(memory_t memory) {
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE ap_ctrl_none port=return

	Pipeline pipeline;
	uint16_t i = 32;
	bool stop;
	while (i--) {
		pipeline.interface(memory, &stop);
	}
}