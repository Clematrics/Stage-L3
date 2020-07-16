#include "pipeline.hpp"

Pipeline::Pipeline()
	: fetch_stage     (5, 0),
	  counter(0)
{
}

void Pipeline::interface(memory_t memory, bool* stop) {
	#pragma HLS PIPELINE

	fetch_stage     .interface(memory);
	counter++;
	if (counter == 10) *stop = true;
}