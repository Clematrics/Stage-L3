#include "stages/commit.hpp"

CommitStage::CommitStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active)
{
}

void CommitStage::interface() {
	if (is_active()) {

	}
	tick();
}