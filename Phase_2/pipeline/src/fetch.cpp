#include "../include/fetch.hpp"

Fetch::Fetch(word_t program_counter)
	: program_counter(program_counter),
	  active(true)
{
}

void Fetch::fetch(const memory_t memory_0, const memory_t memory_1, word_t next_program_counter, word_t* instruction, word_t* current_program_counter) {
	#pragma HLS INLINE

	if (active) {
		// *instruction = (memory[program_counter + 3] << 24)
		//              | (memory[program_counter + 2] << 16)
		//              | (memory[program_counter + 1] <<  8)
		//              |  memory[program_counter];
		(*instruction)(31, 24) = memory_0[program_counter + 3];
		(*instruction)(23, 16) = memory_0[program_counter + 2];
		(*instruction)(15,  8) = memory_1[program_counter + 1];
		(*instruction)( 7,  0) = memory_1[program_counter    ];
		*current_program_counter = program_counter;
		program_counter = next_program_counter;
	}
	active = !active;
}
