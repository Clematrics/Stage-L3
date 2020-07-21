/* ****************************************************************************
*    Source file for the fetch stage
**************************************************************************** */

#include "stages/fetch.hpp"

#include "debug/debug.hpp"

FetchStage::FetchStage() {}

void FetchStage::interface(memory_t memory, DecodeToFetch& from_decode, FetchToDecode* to_decode, bit_t* fetch_ran) {
	#pragma HLS INLINE

	bit_t do_smth = from_decode.has_next_pc;

	if (do_smth) {
		program_counter = from_decode.next_pc;
		word_t instruction = memory[program_counter];

		to_decode->has_fetched = true;
		to_decode->instruction = instruction;
		to_decode->pc          = program_counter;

		#ifndef __SYNTHESIS__
		Debugger::add_cycle_event({
			{ "Fetch stage",
				{
					{ "Program counter", program_counter.to_uint() },
					{ "Raw instruction", string_hex(instruction.to_uint()) }
				}
			}
		});
		#endif // __SYNTHESIS__
	}
	else {
		to_decode->has_fetched = false;
	}
	*fetch_ran = do_smth;
}