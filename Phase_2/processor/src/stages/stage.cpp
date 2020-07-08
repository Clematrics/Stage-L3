#include "stages/stage.hpp"

Stage::Stage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: cycles_activity_period(cycles_activity_period),
	  cycles_before_active(cycles_before_active)
{
}

void Stage::tick() {
	#ifndef __SYNTHESIS__
	if (cycles_before_active)
		cycles_before_active--;
	else
		cycles_before_active = cycles_activity_period - 1;
	#endif // __SYNTHESIS__
}

bool Stage::is_active() {
	#ifndef __SYNTHESIS__
	return cycles_before_active == 0;
	#else
	return true;
	#endif // __SYNTHESIS__
}