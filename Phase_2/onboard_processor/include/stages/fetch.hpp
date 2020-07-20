#pragma once

/* ****************************************************************************
*    Header file for the fetch stage
**************************************************************************** */

#include "inter_stage.hpp"

class FetchStage {
	bool locked;
	program_counter_t program_counter;
public:
	FetchStage();
	void interface(memory_t memory, DecodeToFetch& from_decode, FetchToDecode* to_decode, bool* locked_dbg);
};