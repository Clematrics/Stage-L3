#include "pipeline.hpp"

Pipeline::Pipeline()
	: fetch_stage     (5, 0),
	  counter(0)
{
}

void Pipeline::interface(memory_t memory, bool* stop) {
	// #pragma HLS PIPELINE

	if (!(*stop)) {
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