/* ****************************************************************************
*    Source file for the fetch stage
**************************************************************************** */

#include "stages/fetch.hpp"

#include "debug/debug.hpp"

FetchStage::FetchStage() :
	locked(false) // The fetch stage is initially unlocked, while the decode stage is locke;
{
}

void FetchStage::interface(memory_t memory, DecodeToFetch& from_decode, FetchToDecode* to_decode, bool* locked_dbg) {
	#pragma HLS INLINE

	if (from_decode.has_next_pc) {
		program_counter = from_decode.next_pc;
		locked = false;
	}
	else {
		locked = true;
	}

	if (!locked) {
		word_t instruction = memory[program_counter];

		to_decode->has_fetched = true;
		to_decode->instruction = instruction;
		to_decode->pc          = program_counter;

		locked = true; // lock the fetch stage to wait for the next pc

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

	*locked_dbg = locked;
}