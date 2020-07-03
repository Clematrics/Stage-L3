#pragma once

#include "../include/common.hpp"

class Fetch {
	bool active;
public:
	Fetch();
	void fetch(const memory_t memory, const word_t& program_counter, word_t* instruction, word_t* current_program_counter);
};
