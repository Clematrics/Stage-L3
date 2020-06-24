#pragma once

#include "../include/common.hpp"

class Fetch {
	word_t program_counter;
	bool active;
public:
	Fetch(word_t program_counter);
	void fetch(const memory_t memory_0, const memory_t memory_1, word_t next_program_counter, word_t* instruction, word_t* current_program_counter);
};
