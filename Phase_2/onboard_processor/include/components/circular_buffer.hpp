#pragma once

#include "common.hpp"
#include "debug/debug.hpp"

template<typename entry_t, uint16_t size, typename ptr_t = entry_t, uint16_t initial_pos = 0>
class CircularBuffer {
	entry_t list[size];
	ptr_t   bot;
	ptr_t   top;
	bit_t   empty;
	bit_t   full;
public:
	CircularBuffer() {
		bot = initial_pos;
		top = initial_pos;
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

	void pop() {
		bot++;
		full = false;
		if (bot == top)
			empty = true;
	}

	void push_and_pop(entry_t new_entry) {
		list[top] = new_entry;
		bot++;
		top++;
	}

	entry_t first() {
		return list[bot];
	}

	bit_t is_empty() {
		return empty;
	}

	bit_t is_full() {
		return full;
	}

	entry_t& operator[](uint16_t index) {
		return list[index];
	}

	#ifndef __SYNTHESIS__
	operator json() {
		json array = json::array();
		for (uint32_t i = bot ; i != top ; i = (i + 1) % size)
			array.push_back(list[i].to_uint());
		return json({
			{ "Empty", empty.to_bool() },
			{ "Full",  full.to_bool()  },
			{ "List",  array           }
		});
	}
	#endif // __SYNTHESIS__
};
