/* ****************************************************************************
*    Source file for the fetch stage
**************************************************************************** */

#include "stages/fetch.hpp"

#include "debug/debug.hpp"

FetchStage::FetchStage() {}

#ifdef DBG_SYNTH
void FetchStage::interface(memory_t memory, const DecodeToFetch& from_decode, const WriteBackToFetch& from_write_back, FetchToDecode* to_decode, FetchStatus* status) {
#else
void FetchStage::interface(memory_t memory, const DecodeToFetch& from_decode, const WriteBackToFetch& from_write_back, FetchToDecode* to_decode) {
#endif
	#pragma HLS inline

	bit_t do_smth = from_decode.has_next_pc || from_write_back.has_next_pc;

	if (do_smth) {
		program_counter    = from_write_back.has_next_pc ? from_write_back.next_pc : from_decode.next_pc;
		word_t instruction = memory[program_counter];

		to_decode->has_fetched = true;
		to_decode->instruction = instruction;
		to_decode->pc          = program_counter;
	}
	else {
		to_decode->has_fetched = false;
	}

	#ifndef __SYNTHESIS__
	Debugger::add_cycle_event("Fetch stage", {
		{ "Program counter", string_all(program_counter.to_uint()) }
	});

	#endif // __SYNTHESIS__
	#ifdef DBG_SYNTH
	status->program_counter = program_counter;
	status->did_smth        = do_smth;
	#endif
}