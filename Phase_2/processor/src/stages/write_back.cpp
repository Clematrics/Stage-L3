#include "stages/write_back.hpp"

WriteBackStage::WriteBackStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active)
{
}

void WriteBackStage::interface() {
	#pragma HLS INLINE

	if (is_active()) {

	}
	tick();
}