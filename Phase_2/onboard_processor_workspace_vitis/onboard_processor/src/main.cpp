/*
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <array>
#include <iostream>
#include <iomanip>
#include <vector>

#include "debug/debug.hpp"
#include "xprocessor.h"

const uint32_t memory_size = 32;
std::vector<uint32_t> memory(memory_size);

void print_array(int* ptr, uint32_t size) {
	std::cout << "{ ";
		if (size != 0)
			std::cout << ptr[0];
		for (uint32_t i = 1; i < size; i++)
			std::cout << ", " << ptr[i];
		std::cout << " }\n";
}

void print_memory() {
	print_array((int*)memory.data(), memory.size());
}

XProcessor processor;

void dump_memory() {
    XProcessor_Read_memory_V_Words(&processor, 0, (int*)memory.data(), memory_size);
}

void set_memory() {
	XProcessor_Write_memory_V_Words(&processor, 0, (int*)memory.data(), memory_size);
}

bool is_running() {
	int d;
	XProcessor_Read_run_Words(&processor, 0, &d, 1);
	return d != 0;
}

void run() {
	int d = true;
	XProcessor_Write_run_Words(&processor, 0, &d, 1);
}

bool stopped() {
	return static_cast<bool>(XProcessor_Get_stopped_V(&processor));
}

#include "sleep.h"

int main() {
	custom_streambuf::buffer_t buf;
	custom_streambuf csbuf(&buf);
    std::ostream os(&csbuf);

	XProcessor_Initialize(&processor, XPAR_XPROCESSOR_0_DEVICE_ID);

	memory[0] = 0x01328233; // add tp, t0, s3    = add x4, x5, x19 : 0b 0000'000 1'0011' 0010'1 000' 0010'0 011'0011   = 0x01 32 82 33
	memory[1] = 0xfff4c013; // xori zero, s1, -1 = xori x0, x9, -1 : 0b 1111'1111'1111' 0100'1 100' 0000'0 001'0011    = 0xff f4 c0 13
	memory[2] = 0xfe242e23; // sw s0, -4(sp)     = sw x8, -4(x2)   : 0b 1111'111 0'0010' 0100'0 010' 1110'0 010'0011   = 0xfe 24 2e 23
	memory[3] = 0x00000fb7; // lui t6, 0x0       = lui x31, 0x0    : 0b 0000'0000'0000'0000'0000' 1111'1 011'0111      = 0x00 00 0f b7
	memory[4] = 0x00000000; // unknown
	memory[5] = 0x0140006f; // jal zero, 20      = jal x0, 20      : 0b 0 000'0001'010 0' 0000'0000' 0000'0 110'1111   = 0x01 40 00 6f
	memory[6] = 0x00105063; // bge zero, ra, 0   = bge x0, x1, 0   : 0b 0 000'000 0'0001' 0000'0 101' 0000' 0 110'0011 = 0x00 10 50 63
	memory[7] = 0x00008067; // ret               = jalr x0, 0(x1)  : 0b 0000'0000'0000' 0000'1 000' 0000'0 110'0111    = 0x00 00 80 67
	memory[8] = 0x00008067; // ret               = jalr x0, 0(x1)  : 0b 0000'0000'0000' 0000'1 000' 0000'0 110'0111    = 0x00 00 80 67
	memory[9] = 0x00008067; // ret               = jalr x0, 0(x1)  : 0b 0000'0000'0000' 0000'1 000' 0000'0 110'0111    = 0x00 00 80 67

	set_memory();

	os << "[\n";
	run(); // Launch the IP
    do {
		while (is_running()); // wait for the IP to finish a cycle
		usleep(1000);

		// dump_memory();
		// print_memory();
		os << std::boolalpha << std::setw(4) << dump_debug_info(&processor) << ",\n";

		run();
	} while (!stopped());
	os << ']';

    return 0;
}
