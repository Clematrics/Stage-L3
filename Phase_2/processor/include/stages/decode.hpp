#pragma once

#include "stage.hpp"
#include "inter_stage.hpp"
#include "components/register_map.hpp"

class DecodeStage : Stage {
	RegisterMap register_map;
	reorder_buffer_ptr_t token;
public:
	DecodeStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface(const FetchToDecode& from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, DecodeToCommit* to_commit, bool* stop);
};