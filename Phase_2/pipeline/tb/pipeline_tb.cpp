#include <cstdint>
#include <iostream>

#include "../include/common.hpp"
#include "../include/config.hpp"
#include "../include/pipeline.hpp"

int main() {
	uint8_t memory[memory_bytes];

	for (std::size_t i = 0; i < memory_words; i++) {
		memory[4 * i]     = 0;
		memory[4 * i + 1] = 0;
		memory[4 * i + 2] = 0;
		memory[4 * i + 3] = 0;
	}
	memory[4 * 10]     = 0xff;
	memory[4 * 10 + 1] = 0xff;
	memory[4 * 10 + 2] = 0xff;
	memory[4 * 10 + 3] = 0xff;

	Pipeline pipeline;

	uint32_t cycles = 0;
	bool stop = false;
	while (!stop) {
		pipeline.pipeline(memory, &stop);
		cycles++;
	}

	std::cout << "Cycles count : " << cycles << '\n';

	return 0;
}
