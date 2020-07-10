#pragma once

#include "stage.hpp"
#include "units/alu.hpp"

class WriteBackStage : Stage {
	Alu alu;
public:
	WriteBackStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface();
};