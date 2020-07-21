/* ****************************************************************************
*    Source file for the decode stage
**************************************************************************** */

#include "stages/decode.hpp"

#include "debug/debug.hpp"

DecodeStage::DecodeStage() {}

void DecodeStage::interface(FetchToDecode& from_fetch, DecodeToFetch* to_fetch, bit_t* stop, bit_t* decode_ran) {
	#pragma HLS INLINE

	bit_t do_smth = from_fetch.has_fetched;

	if (do_smth) {
		program_counter_t next_pc = from_fetch.pc + 1;

		to_fetch->has_next_pc = true;
		to_fetch->next_pc     = next_pc;

		if (next_pc == memory_words)
			*stop = true;

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
	*decode_ran = do_smth;
}