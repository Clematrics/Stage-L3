#include "stages/decode.hpp"

DecodeStage::DecodeStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active)
{
}

void DecodeStage::interface() {
	if (is_active()) {

	}
	tick();
}