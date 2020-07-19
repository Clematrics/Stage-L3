#include "processor.hpp"

#include "common.hpp"
#include "debug/debug.hpp"
#include "pipeline.hpp"

#define DEBUG

#define TW(type) tw_##type
#define DECL_TW_TYPE(type) typedef type TW(type)[1]

typedef uint32_t large_bool;
DECL_TW_TYPE(large_bool);

void processor(memory_t memory, TW(large_bool) hold, TW(large_bool) stop) {
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE s_axilite port=hold
	#pragma HLS INTERFACE s_axilite port=stop
	#pragma HLS INTERFACE ap_ctrl_none port=return

	word_t counter = 0;
	while (! *stop) {
		#if defined(__SYNTHESIS__) && defined(DEBUG)
		if (! *hold) {
			// Waiting for the 'hold' signal to be false to do a cycle
		#endif
			#ifndef __SYNTHESIS__
			Debugger::new_cycle();
			#endif // __SYNTHESIS__

			memory[counter] = counter;
			counter++;

		#if defined(__SYNTHESIS__) && defined(DEBUG)
			// Cycle has ended, hold is set to trrue
			*hold = true;
		}
		#endif
	}
}