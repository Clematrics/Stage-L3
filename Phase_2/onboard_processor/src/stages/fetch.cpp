#include "stages/fetch.hpp"

#include <string>

#include "debug/debug.hpp"

FetchStage::FetchStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active),
	  program_counter(0)
{
}

void FetchStage::interface(memory_t memory) {
	#pragma HLS INLINE

	if (is_active()) {

	}
	tick();
}