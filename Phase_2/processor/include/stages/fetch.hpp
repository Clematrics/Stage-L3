#pragma once

#include "stage.hpp"
#include "inter_stage.hpp"

class FetchStage : Stage {
	program_counter_t program_counter;
public:
	FetchStage(uint16_t cycles_activity_period, uint16_t cycles_before_active);
	void interface(const memory_t memory, DecodeToFetch from_decode, WriteBackToFetch from_write_back, FetchToDecode* to_decode);
};