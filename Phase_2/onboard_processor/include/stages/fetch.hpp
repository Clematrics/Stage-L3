#pragma once

/* ****************************************************************************
*    Header file for the fetch stage
**************************************************************************** */

#include "config.hpp"
#include "debug/debug.hpp"
#include "stage_structs.hpp"

class FetchStage {
	program_counter_t program_counter;
public:
	FetchStage();
	#ifdef DBG_SYNTH
	void interface(memory_t memory, const DecodeToFetch& from_decode, const WriteBackToFetch& from_write_back, FetchToDecode* to_decode, FetchStatus* status);
	#else
	void interface(memory_t memory, const DecodeToFetch& from_decode, const WriteBackToFetch& from_write_back, FetchToDecode* to_decode);
	#endif
};