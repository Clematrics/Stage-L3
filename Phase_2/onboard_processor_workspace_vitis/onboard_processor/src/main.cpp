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

bool get_stop() {
	int d;
	XProcessor_Read_stop_Words(&processor, 0, &d, 1);
	return d != 0;
}

void set_stop(bool value) {
	int d = value;
	XProcessor_Write_stop_Words(&processor, 0, &d, 1);
}


int main() {
	XProcessor_Initialize(&processor, XPAR_XPROCESSOR_0_DEVICE_ID);

	memory[0] = 0x01328233;
	memory[1] = 0xfff4c013;
	memory[2] = 0xfe242e23;
	memory[3] = 0x00105063;
	memory[4] = 0x00000fb7;
	memory[5] = 0x0080006f;
	memory[6] = 0x00000000;
	memory[7] = 0x00008067;
	memory[8] = 0x00008067;
	memory[9] = 0x00008067;

	set_memory();

	run(); // Launch the IP
    do {
		while (is_running()); // wait for the IP to finish a cycle

		dump_memory();
		print_memory();
		dumpDebugInfo(&processor);

		run();
	} while (!get_stop());

    return 0;
}
