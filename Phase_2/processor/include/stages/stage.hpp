#pragma once

#include "common.hpp"

class Stage {
	uint16_t cycles_activity_period;
	uint16_t cycles_before_active;
public:
	Stage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
protected:
	void tick();
	bool is_active();
};