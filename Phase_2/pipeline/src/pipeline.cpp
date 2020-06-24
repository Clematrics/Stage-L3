#include "../include/pipeline.hpp"

Pipeline::Pipeline(word_t initial_program_counter)
	: fetch_stage(initial_program_counter),
	  instruction(0),
	  program_counter(initial_program_counter),
	  next_program_counter(initial_program_counter)
{
}

void Pipeline::pipeline(memory_t memory_1, memory_t memory_2, bit_t* stop) {
	#pragma HLS PIPELINE
	fetch_stage.fetch(memory_1, memory_2, next_program_counter, &instruction, &program_counter);
	decode_stage.decode(instruction, program_counter, &next_program_counter, stop, &decoded);
}


// top function
void pipeline(memory_t memory_1, memory_t memory_2, word_t* cycles) {
	#pragma HLS INTERFACE s_axilite port=memory_1
	#pragma HLS INTERFACE s_axilite port=memory_2
	#pragma HLS INTERFACE s_axilite port=cycle
	#pragma HLS INTERFACE ap_ctrl_none port=return
	Pipeline pipeline;
	bit_t stop;
	while(!stop) {
		pipeline.pipeline(memory_1, memory_2, &stop);
		(*cycles)++;
	}
}
