#include "../include/decode.hpp"

Decode::Decode() {}

void Decode::decode(const word_t instruction, word_t program_counter, word_t* out_program_counter, bool* stop_signal) {
	#pragma HLS INLINE
	*out_program_counter = program_counter + sizeof(word_t);
	*stop_signal = instruction == 0xffffffff ? true : false;
}
