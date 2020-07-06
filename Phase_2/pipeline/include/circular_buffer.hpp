#pragma once

#include "../include/json.hpp"

template<typename entry_t, int size, typename ptr_t = entry_t>
class CircularBuffer {
	entry_t list[size];
	ptr_t   bot;
	ptr_t   top;
	bool    empty;
	bool    full;
public:
	CircularBuffer();
	void push_back(entry_t entry);
	entry_t pop();
	bool is_empty();
	bool is_full();

	#ifndef __SYNTHESIS__
	operator json();
	#endif // __SYNTHESIS__
};

template<typename entry_t, int size, typename ptr_t>
CircularBuffer::CircularBuffer() {
	bot = 0;
	top = 0;
	empty = true;
	full = false;
}

template<typename entry_t, int size, typename ptr_t>
void CircularBuffer::push_back(entry_t entry) {
	list[top] = entry;
	top++;
	empty = false;
	if (bot == top)
		full = true;
}

template<typename entry_t, int size, typename ptr_t>
entry_t CircularBuffer::pop() {
	entry_t value = list[bot];
	bot--;
	full = false;
	if (bot == top)
		empty = true;
	return value;
}

template<typename entry_t, int size, typename ptr_t>
bool CircularBuffer::is_empty() {
	return empty;
}

template<typename entry_t, int size, typename ptr_t>
bool CircularBuffer::is_full() {
	return full;
}

#ifndef __SYNTHESIS__
template<typename entry_t, int size, typename ptr_t>
CircularBuffer::operator json() {
	json array = json::array();
	for (std::size_t i = bot ; i != top ; i = (i + 1) % size)
		array.push_back(list[i].to_uint());
	return json({
		{ "Empty", empty },
		{ "Full", full },
		{ "List", array }
	});
}
#endif // __SYNTHESIS__