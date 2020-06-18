#pragma once

#include "common.hpp"

class Decode {

public:
	Decode();
	void decode(const word_t instruction, word_t program_counter, word_t* out_program_coutner);
};
