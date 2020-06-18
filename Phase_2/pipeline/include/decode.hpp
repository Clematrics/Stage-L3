#pragma once

#include "../include/common.hpp"

class Decode {

public:
	Decode();
	void decode(const word_t instruction, word_t program_counter, word_t* out_program_counter, bool* stop_signal);
};
