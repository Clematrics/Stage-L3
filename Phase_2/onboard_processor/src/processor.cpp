#include "processor.hpp"

#include "common.hpp"
#include "debug/debug.hpp"
#include "pipeline.hpp"

#define DEBUG

#define TW(type) tw_##type
#define DECL_TW_TYPE(type) typedef type TW(type)[1]

typedef uint32_t large_bool;
DECL_TW_TYPE(large_bool);

struct Test {
	word_t test_info;
};

struct DebugInfo {
	word_t info;
	Test test;
};

#if defined(__SYNTHESIS__) && defined(DEBUG)
void processor(memory_t memory, TW(large_bool) hold, TW(large_bool) stop, DebugInfo* dbg) {
#else
void processor(memory_t memory, TW(large_bool) stop) {
#endif
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE s_axilite port=hold
	#pragma HLS INTERFACE s_axilite port=stop
	#pragma HLS INTERFACE s_axilite port=dbg
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
			dbg->info = counter;
			counter++;
			dbg->test.test_info = counter;

		#if defined(__SYNTHESIS__) && defined(DEBUG)
			// Cycle has ended, hold is set to trrue
			*hold = true;
		}
		#endif
	}
}