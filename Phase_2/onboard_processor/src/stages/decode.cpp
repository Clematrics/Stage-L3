/* ****************************************************************************
*    Source file for the decode stage
**************************************************************************** */

#include "stages/decode.hpp"

#include "debug/debug.hpp"

DecodeStage::DecodeStage() :
	locked(true) // The decode stage is initially locked, while the fetch stage is unlocked
{
}

void DecodeStage::interface(FetchToDecode& from_fetch, DecodeToFetch* to_fetch, large_bool* stop, bool* locked_dbg) {
	#pragma HLS INLINE

	locked = !from_fetch.has_fetched;

	if (!locked) {
		program_counter_t next_pc = from_fetch.pc + 1;

		to_fetch->has_next_pc = true;
		to_fetch->next_pc     = next_pc;

		if (next_pc == memory_words)
			*stop = true;

		locked = true; // lock the decode stage to wait for the next instruction

		#ifndef __SYNTHESIS__
		Debugger::add_cycle_event({
			{ "Decode stage",
				{
					{ "Next pc", to_fetch->next_pc.to_uint() }
				}
			}
		});
		#endif // __SYNTHESIS__
	}
	else {
		to_fetch->has_next_pc = false;
	}

	*locked_dbg = locked;
}