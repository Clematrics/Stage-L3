#include <cstdint>
#include <iostream>

#include "../include/common.hpp"
#include "../include/pipeline.hpp"

int main() {
	memory_t memory;

	for (std::size_t i = 0; i < memory_words; i++) {
		memory[4 * i]     = 0;
		memory[4 * i + 1] = 0;
		memory[4 * i + 2] = 0;
		memory[4 * i + 3] = 0;
	}

	// add x4, x5, x19 : 0b 0000'000 1'0011' 0010'1 000' 0010'0 011'0011 = 0x01 32 82 33
	memory[4 *  0]     = 0x33;
	memory[4 *  0 + 1] = 0x82;
	memory[4 *  0 + 2] = 0x32;
	memory[4 *  0 + 3] = 0x01;
	// xori x0, x9, -1: 0b 1111'1111'1111' 0100'1 100' 0000'0 001'0011 = 0xff f4 c0 13
	memory[4 *  1]     = 0x13;
	memory[4 *  1 + 1] = 0xc0;
	memory[4 *  1 + 2] = 0xf4;
	memory[4 *  1 + 3] = 0xff;
	// sw x8, -4(x2) : 0b 1111'111 0'0010' 0100'0 010' 1110'0 010'0011 = 0xfe 24 2e 23
	memory[4 *  2]     = 0x23;
	memory[4 *  2 + 1] = 0x2e;
	memory[4 *  2 + 2] = 0x24;
	memory[4 *  2 + 3] = 0xfe;
	// bge x0, x1, 0 : 0b 0 000'000 0'0001' 0000'0 101' 0000' 0 110'0011 = 0x00 10 50 63
	memory[4 *  3]     = 0x63;
	memory[4 *  3 + 1] = 0x50;
	memory[4 *  3 + 2] = 0x10;
	memory[4 *  3 + 3] = 0x00;
	// lui x31, 0x0 : 0b 0000'0000'0000'0000'0000' 1111'1 011'0111 = 0x00 00 0f b7
	memory[4 *  4]     = 0xb7;
	memory[4 *  4 + 1] = 0x0f;
	memory[4 *  4 + 2] = 0x00;
	memory[4 *  4 + 3] = 0x00;
	// jal x0, 4 : 0b 0 000'0000'010 0' 0000'0000' 0000'0 110'1111 = 0x00 40 00 6f
	memory[4 *  5]     = 0x6f;
	memory[4 *  5 + 1] = 0x00;
	memory[4 *  5 + 2] = 0x40;
	memory[4 *  5 + 3] = 0x00;
	// invalid from 6 to 9
	// ret (jalr x0, 0(x1)) : 0b 0000'0000'0000' 0000'1 000' 0000'0 110'0111 = 0x00 00 80 67
	memory[4 * 10]     = 0x67;
	memory[4 * 10 + 1] = 0x80;
	memory[4 * 10 + 2] = 0x00;
	memory[4 * 10 + 3] = 0x00;

	word_t cycles = 0;
	// pipeline(memory, memory, memory, memory, &cycles);
	pipeline(memory, memory, &cycles);

	std::cout << "Cycles count : " << cycles << '\n';

	return 0;
}
