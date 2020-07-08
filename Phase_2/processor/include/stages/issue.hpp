#pragma once

#include "stage.hpp"

class IssueStage : Stage {
public:
	IssueStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface();
};