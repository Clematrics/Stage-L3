#pragma once

#include "common.hpp"

enum Operation { load = 0, store = 1 };

class MemoryUnit {
public:
	struct Input {
		word_t effective_address;
		Operation operation;
		word_t to_store_value;
	};

	struct Output {
		bool is_busy;
		Operation operation;
		word_t loaded_value;
	};

	MemoryUnit();
	void interface(memory_t memory, Input input, Output* output);
};