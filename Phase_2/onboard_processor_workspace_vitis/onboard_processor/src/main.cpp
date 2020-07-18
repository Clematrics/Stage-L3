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

void print_memory() {
	std::cout << "{ ";
	if (memory_size != 0)
		std::cout << memory[0];
	for (uint32_t i = 1; i < memory_size; i++)
		std::cout << ", " << memory[i];
	std::cout << " }\n";
}

int main()
{
	XProcessor processor;
	XProcessor_Initialize(&processor, XPAR_XPROCESSOR_0_DEVICE_ID);

	XProcessor_Set_stop_i(&processor, false);

    while (!XProcessor_Get_stop_o(&processor)) { }

    XProcessor_Read_memory_V_Words(&processor, 0, (int*)memory.data(), memory_size);
	print_memory();

    return 0;
}
