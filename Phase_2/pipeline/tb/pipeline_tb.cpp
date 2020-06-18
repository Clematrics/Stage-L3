#include <cstdint>
#include <iostream>

#include "common.hpp"
#include "pipeline.hpp"

constexpr uint32_t memory_words = 4096;
constexpr uint8_t  word_size    = sizeof(word_t);
constexpr uint32_t memory_bytes = memory_words * word_size;

int main() {
	uint8_t memory[memory_bytes];

	Pipeline pipeline(memory);

	uint32_t cycles = 0;
	bool stop = false;
	while (!stop) {
		pipeline.pipeline(memory, &stop);
		cycles++;
	}
}
