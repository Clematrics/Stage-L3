#pragma once

#include "stage.hpp"

class CommitStage : Stage {
public:
	CommitStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface();
};