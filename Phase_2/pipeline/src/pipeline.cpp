#include "../include/pipeline.hpp"

#include "../include/debugger.hpp"

Pipeline::Pipeline(word_t initial_program_counter)
	: fetch_stage(initial_program_counter),
	  instruction(0),
	  program_counter(initial_program_counter),
	  next_program_counter(initial_program_counter)
{
}

void Pipeline::pipeline(memory_t memory, bit_t* stop) {
	#pragma HLS PIPELINE
	fetch_stage.fetch(memory, next_program_counter, &instruction, &program_counter);
	decode_stage.decode(instruction, program_counter, &next_program_counter, stop, &decoded);
}


// top function
void pipeline(memory_t memory) {
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE ap_ctrl_none port=return
	Pipeline pipeline;
	bit_t stop;
	while(!stop) {
		#ifndef __SYNTHESIS__
		Debugger::new_cycle();
		#endif // __SYNTHESIS__
		pipeline.pipeline(memory, &stop);
	}
}
