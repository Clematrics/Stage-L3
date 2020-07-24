#pragma once

/* ****************************************************************************
*    Header file for the fetch stage
**************************************************************************** */

#include "stage_structs.hpp"

class FetchStage {
	program_counter_t program_counter;
public:
	FetchStage();
	void interface(memory_t memory, DecodeToFetch& from_decode, FetchToDecode* to_decode, bit_t* fetch_ran);
};