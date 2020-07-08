#pragma once

#include "stage.hpp"

class DecodeStage : Stage {
public:
	DecodeStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface();
};