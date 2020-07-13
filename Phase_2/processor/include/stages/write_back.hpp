#pragma once

#include "stage.hpp"

class WriteBackStage : Stage {
public:
	WriteBackStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface();
};