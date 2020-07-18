#pragma once

#include "stages/fetch.hpp"

class Pipeline {
	FetchStage fetch_stage;
	int counter;
public:
	Pipeline();
	void interface(memory_t memory, bool* stop);
};