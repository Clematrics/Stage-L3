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
#include <vector>
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

bool get_hold() {
	int d;
	XProcessor_Read_hold_Words(&processor, 0, &d, 1);
	return d != 0;
}

void set_hold(bool value) {
	int d = value;
	XProcessor_Write_hold_Words(&processor, 0, &d, 1);
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

	dump_memory();
	print_memory();
    while (!get_stop()) {
		while (!get_hold()); // wait for the IP to finish a cycle

		dump_memory();
		print_memory();
		std::cout << XProcessor_Get_dbg_info_V(&processor) << ' ' << XProcessor_Get_dbg_test_test_info_V(&processor) << '\n';

		set_hold(false);
	}

    return 0;
}
