#include "../include/fetch.hpp"

#include <string>

#include "../include/debugger.hpp"
#include "../include/disasm.hpp"

Fetch::Fetch()
	: active(true)
{
}

void Fetch::fetch(const memory_t memory, const word_t& program_counter, word_t* instruction, word_t* current_program_counter) {
	#pragma HLS INLINE

	if (active) {
		*instruction = memory[program_counter];
		*current_program_counter = program_counter;
		#ifndef __SYNTHESIS__
		std::string asm_line = instruction_to_string(disassemble(*instruction));
		Debugger::add_asm_line(asm_line);
		Debugger::add_cycle_event({
			{ "Fetch stage",
				{
					{ "Program counter", program_counter.to_uint() },
					{ "Raw", string_hex(instruction->to_uint()) },
					{ "Instruction", asm_line }
				}
			}
		});
		#endif // __SYNTHESIS__
	}
	active = !active;
}
