#pragma once

#include "common.hpp"
#include "fetch.hpp"
#include "decode.hpp"

class Pipeline {
	Fetch fetch_stage;
	Decode decode_stage;
public:
	Pipeline(word_t program_counter = 0);
	void pipeline(memory_t memory, bool* stop);
};
