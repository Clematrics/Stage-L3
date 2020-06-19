#include "../include/pipeline.hpp"

Pipeline::Pipeline(word_t initial_program_counter)
	: fetch_stage(initial_program_counter),
	  instruction(0),
	  program_counter(initial_program_counter),
	  next_program_counter(initial_program_counter)
{
}

void Pipeline::pipeline(memory_t memory, bool* stop) {
	#pragma HLS PIPELINE
	fetch_stage.fetch(memory, next_program_counter, &instruction, &program_counter);
	decode_stage.decode(instruction, program_counter, &next_program_counter, stop);
}


// top function
#include "../include/config.hpp"

void pipeline(memory_t memory, uint32_t* cycles) {
	#pragma HLS INTERFACE s_axilite port=memory
	#pragma HLS INTERFACE s_axilite port=cycle
	#pragma HLS INTERFACE ap_ctrl_none port=return
	Pipeline pipeline;
	bool stop;
	while(!stop) {
		pipeline.pipeline(memory, &stop);
		(*cycles)++;
	}
}
