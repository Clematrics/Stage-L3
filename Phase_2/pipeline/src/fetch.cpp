#include "../include/fetch.hpp"

Fetch::Fetch(word_t program_counter)
	: program_counter(program_counter)
{
}

void Fetch::fetch(const memory_t memory, word_t next_program_counter, word_t* instruction, word_t* current_program_counter) {
	*instruction = (memory[program_counter + 3] << 24)
	             | (memory[program_counter + 2] << 16)
	             | (memory[program_counter + 1] << 8)
	             |  memory[program_counter];
	*current_program_counter = program_counter;
	program_counter = next_program_counter;
}