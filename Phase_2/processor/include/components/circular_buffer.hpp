#pragma once

#include "common.hpp"
#include "debug/debug.hpp"

template<typename entry_t, int size, typename ptr_t = entry_t>
class CircularBuffer {
	entry_t list[size];
	ptr_t   bot;
	ptr_t   top;
	bool    empty;
	bool    full;
public:
	CircularBuffer() {
		bot = 0;
		top = 0;
		empty = true;
		full = false;
	}
	void push_back(entry_t entry) {
		list[top] = entry;
		top++;
		empty = false;
		if (bot == top)
			full = true;
	}
	entry_t pop() {
		entry_t value = list[bot];
		bot--;
		full = false;
		if (bot == top)
			empty = true;
		return value;
	}
	bool is_empty() {
		return empty;
	}
	bool is_full() {
		return full;
	}

	#ifndef __SYNTHESIS__
	operator json() {
		json array = json::array();
		for (uint32_t i = bot ; i != top ; i = (i + 1) % size)
			array.push_back(list[i].to_uint());
		return json({
			{ "Empty", empty },
			{ "Full", full },
			{ "List", array }
		});
	}
	#endif // __SYNTHESIS__
};
