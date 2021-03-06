#include "stages/fetch.hpp"

#include <string>

#include "architecture/disasm.hpp"
#include "debug/debug.hpp"

FetchStage::FetchStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active),
	  program_counter(0)
{
}

void FetchStage::interface(const memory_t memory, const DecodeToFetch& from_decode, WriteBackToFetch from_write_back, FetchToDecode* to_decode) {
	#pragma HLS INLINE

	if (is_active()) {
		if (from_decode.block_fetch_stage) {
			to_decode->has_fetched_instruction = false;
		}
		else {
			program_counter = from_decode.next_program_counter;

			to_decode->has_fetched_instruction = true;
			to_decode->instruction             = memory[program_counter];
			to_decode->program_counter         = program_counter;

			#ifndef __SYNTHESIS__
			std::string asm_line = instruction_to_string(disassemble(to_decode->instruction));
			Debugger::add_asm_line(asm_line);
			Debugger::add_cycle_event({
				{ "Fetch stage",
					{
						{ "Program counter", program_counter.to_uint() },
						{ "Raw", string_hex(to_decode->instruction.to_uint()) },
						{ "Instruction", asm_line }
					}
				}
			});
			#endif // __SYNTHESIS__
		}
	}
	tick();
}