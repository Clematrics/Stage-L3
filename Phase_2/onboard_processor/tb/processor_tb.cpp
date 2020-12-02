/* ****************************************************************************
*    Main testbench to check the implementation behavior in Vivado/Vitis HLS
**************************************************************************** */

#include <exception>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "common.hpp"
#include "debug/debug.hpp"
#include "processor.hpp"

#ifndef __SYNTHESIS__
int main() {
	memory_t memory;

	for (uint32_t i = 0; i < memory_words; i++) {
		memory[i] = 0;
	}

	memory[0] = 0x01328233; // add tp, t0, s3    = add x4, x5, x19 : 0b 0000'000 1'0011' 0010'1 000' 0010'0 011'0011   = 0x01 32 82 33
	memory[1] = 0xfff4c013; // xori zero, s1, -1 = xori x0, x9, -1 : 0b 1111'1111'1111' 0100'1 100' 0000'0 001'0011    = 0xff f4 c0 13
	memory[2] = 0x00242c23; // sw s0, 24(sp)     = sw x8, 24(x2)   : 0b 0000'000 0'0010' 0100'0 010' 1100'0 010'0011   = 0x00 24 2c 23
	memory[3] = 0x00000fb7; // lui t6, 0x0       = lui x31, 0x0    : 0b 0000'0000'0000'0000'0000' 1111'1 011'0111      = 0x00 00 0f b7
	memory[4] = 0x00000000; // unknown
	memory[5] = 0x0140006f; // jal zero, 20      = jal x0, 20      : 0b 0 000'0001'010 0' 0000'0000' 0000'0 110'1111   = 0x01 40 00 6f
	memory[6] = 0x00105063; // bge zero, ra, 0   = bge x0, x1, 0   : 0b 0 000'000 0'0001' 0000'0 101' 0000' 0 110'0011 = 0x00 10 50 63
	memory[7] = 0x00008067; // ret               = jalr x0, 0(x1)  : 0b 0000'0000'0000' 0000'1 000' 0000'0 110'0111    = 0x00 00 80 67
	memory[8] = 0x00008067; // ret               = jalr x0, 0(x1)  : 0b 0000'0000'0000' 0000'1 000' 0000'0 110'0111    = 0x00 00 80 67
	memory[9] = 0x00008067; // ret               = jalr x0, 0(x1)  : 0b 0000'0000'0000' 0000'1 000' 0000'0 110'0111    = 0x00 00 80 67

	try {
		bit_t stop = false;
		processor(memory, &stop);
	}
	catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << '\n';
	}

	std::ofstream out;
	out.open("../../../output.json");
	out << std::setw(4) << Debugger::get_report();
	out.close();

	return 0;
}
#endif // __SYNTHESIS__