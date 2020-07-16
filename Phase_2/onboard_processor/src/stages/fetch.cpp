#include "stages/fetch.hpp"

#include <string>

#include "debug/debug.hpp"

FetchStage::FetchStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active),
	  program_counter(0)
{
}

void FetchStage::interface(memory_t memory) {
	#pragma HLS INLINE

	if (is_active()) {

		memory[program_counter + 10] = memory[program_counter];
		program_counter++;

		#ifndef __SYNTHESIS__
		// std::string asm_line = instruction_to_string(disassemble(to_decode->instruction));
		// Debugger::add_asm_line(asm_line);
		// Debugger::add_cycle_event({
		// 	{ "Fetch stage",
		// 		{
		// 			{ "Program counter", program_counter.to_uint() },
		// 			{ "Raw", string_hex(to_decode->instruction.to_uint()) },
		// 			{ "Instruction", asm_line }
		// 		}
		// 	}
		// });
		#endif // __SYNTHESIS__
	}
	tick();
}