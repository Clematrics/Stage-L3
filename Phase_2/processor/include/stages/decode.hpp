#pragma once

#include "stage.hpp"
#include "inter_stage.hpp"
#include "components/register_map.hpp"

class DecodeStage : Stage {
	RegisterMap register_map;
public:
	DecodeStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface(FetchToDecode from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, bool* stop);
};