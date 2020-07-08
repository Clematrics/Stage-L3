#include "stages/issue.hpp"

IssueStage::IssueStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active)
{
}

void IssueStage::interface() {
	if (is_active()) {

	}
	tick();
}