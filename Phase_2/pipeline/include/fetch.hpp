#pragma once

#include "../include/common.hpp"

class Fetch {
	word_t program_counter;
public:
	Fetch(word_t program_counter);
	void fetch(const memory_t memory, word_t next_program_counter, word_t* instruction, word_t* current_program_counter);
};
