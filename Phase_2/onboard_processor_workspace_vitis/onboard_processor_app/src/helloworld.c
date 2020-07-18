/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
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

#include <stdio.h>
#include <stdbool.h>
#include "platform.h"
#include "xil_printf.h"
#include "xprocessor.h"

#define MEMORY_SIZE 32

void init_memory(unsigned int* ptr, unsigned int size) {
	// for (int i = 0; i < size; i++)
	//	ptr[i] = 0;
}

void print_memory(unsigned int* ptr, unsigned int size) {
	xil_printf("{ ");
	if (size != 0)
		xil_printf("%lu", ptr[0]);
	for (int i = 1; i < size; i++)
		xil_printf(", %lu", ptr[i]);
	xil_printf(" }");
}

int main()
{
    unsigned int* memory = (unsigned int *)(XPAR_PROCESSOR_0_S_AXI_AXILITES_BASEADDR + XPROCESSOR_AXILITES_ADDR_MEMORY_V_BASE);
    unsigned int* stop_i = (unsigned int *)(XPAR_PROCESSOR_0_S_AXI_AXILITES_BASEADDR + XPROCESSOR_AXILITES_ADDR_STOP_I_DATA);
    unsigned int* stop_o = (unsigned int *)(XPAR_PROCESSOR_0_S_AXI_AXILITES_BASEADDR + XPROCESSOR_AXILITES_ADDR_STOP_O_DATA);


	*stop_i = 0;
	init_memory(memory, MEMORY_SIZE);

    init_platform();

    while (!(*stop_o & 0x1)) {
    }

	print_memory(memory, MEMORY_SIZE);

    cleanup_platform();
    return 0;

    // u32 ret;

	// XProcessor         proc_instance;
	// XProcessor_Config* cfg_instance_ptr;
	// cfg_instance_ptr = XProcessor_LookupConfig(0);
	// XProcessor_CfgInitialize(&proc_instance, cfg_instance_ptr);
	// if (proc_instance.IsReady != XIL_COMPONENT_IS_READY) {
	// 	print("Not ready yet?\n\r");
	// }

	// XProcessor_Set_stop_i(&proc_instance, 0);
	// int memory[MEMORY_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	// ret = XProcessor_Write_memory_V_Words(&proc_instance, 0, memory, MEMORY_SIZE);
	// xil_printf("Length written to memory : %lu\n\r", ret);

    // init_platform();

	// bool stop = false;
    // while (!stop) {
    // 	u32 s = XProcessor_Get_stop_o(&proc_instance);
    // 	stop = s;
    // }

    // XProcessor_Read_memory_V_Words(&proc_instance, 0, memory, MEMORY_SIZE);
	// for (int i = 0; i < MEMORY_SIZE; i++)
	// 	xil_printf("%lu\n\r", memory[i]);

    // cleanup_platform();
    // return 0;
}
