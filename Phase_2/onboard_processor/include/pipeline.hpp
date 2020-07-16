#pragma once

#include "stages/fetch.hpp"

class Pipeline {
	FetchStage fetch_stage;
	uint8_t counter;
public:
	Pipeline();
	void interface(memory_t memory, bool* stop);
};