/* ****************************************************************************
*    Source file for the decode stage
**************************************************************************** */

#include "stages/decode.hpp"

#include "architecture/decoding.hpp"
#include "architecture/immediate_packing.hpp"
#include "architecture/architecture_helpers.hpp"
#include "debug/debug.hpp"

using namespace Architecture::Type;
using namespace Architecture::InstructionCategory;
using namespace Decoding;

DecodeStage::DecodeStage() {
	#pragma array_partition variable=register_map complete
	#pragma array_partition variable=free_aliases complete

	token           = initial_token;
	ready_registers = 0;

	// Initialization of the register map
	for (uint16_t id = 0; id < architectural_register_count; id++) {
		#pragma HLS unroll
		register_map[id] = id;
	}

	// Initialization of the ready flags
	ready_registers = -1; // all registers are marked as ready so computing can begin

	// Initialization of the free aliases - 0 must NOT be in the free aliases
	for (uint16_t id = architectural_register_count; id < physical_register_count; id++) {
		#pragma HLS unroll
		free_aliases[id - architectural_register_count] = id;
	}

	free_aliases_bot = 0;
	free_aliases_top = physical_register_count - architectural_register_count;

	free_aliases_empty = false;
	if (free_aliases_bot == free_aliases_top)
		free_aliases_full = true;
}

void DecodeStage::free_aliases_pop() {
	#pragma HLS inline

	free_aliases_bot++;
	free_aliases_full = false;
	if (free_aliases_bot == free_aliases_top)
		free_aliases_empty = true;
}

void DecodeStage::free_aliases_push(physical_reg_t entry) {
	#pragma HLS inline
	#pragma array_partition variable=free_aliases complete

	free_aliases[free_aliases_top] = entry;
	free_aliases_top++;
	free_aliases_empty = false;
	if (free_aliases_bot == free_aliases_top)
		free_aliases_full = true;
}

void DecodeStage::get_register_alias(const reg_t& id, physical_reg_t* alias) {
	#pragma HLS inline
	#pragma array_partition variable=register_map complete

	*alias = register_map[id];
}

void DecodeStage::create_register_alias(const reg_t& id, physical_reg_t* alias, bit_t* blocking) {
	#pragma HLS inline
	#pragma array_partition variable=register_map complete
	#pragma array_partition variable=free_aliases complete

	if (free_aliases_empty) {
		*blocking = true;
	}
	else {
		physical_reg_t new_alias = free_aliases[free_aliases_bot];
		free_aliases_pop();
		*alias = register_map[id] = new_alias;
		*blocking = free_aliases_empty;
	}

	// #ifndef __SYNTHESIS__
	// json json_map = json::object();
	// for (uint16_t id = 0 ; id < architectural_register_count ; id++) {
	// 	json_map.push_back({ to_string(static_cast<reg_t>(id)), register_map[id].to_uint() });
	// }

	// json array = json::array();
	// FOR_CYCLE_BUFFER(free_aliases, physical_register_count) {
	// 	array.push_back(free_aliases[i].to_uint());
	// }

	// Debugger::add_cycle_event("Creating alias", {
	// 	{ "Architectural register",    to_string(id)    },
	// 	{ "Physical register created", alias->to_uint() },
	// 	{ "Current mapping",           json_map         },
	// 	{ "Free registers", {
	// 		{ "Empty", free_aliases_empty.to_bool() },
	// 		{ "Full",  free_aliases_full.to_bool()  },
	// 		{ "List",  array                        }
	// 	} }
	// });
	// #endif // __SYNTHESIS__
}

void DecodeStage::free_register_alias(const physical_reg_t& id) {
	#pragma HLS inline
	free_aliases_push(id);

	// #ifndef __SYNTHESIS__
	// json array = json::array();
	// FOR_CYCLE_BUFFER(free_aliases, physical_register_count) {
	// 	array.push_back(free_aliases[i].to_uint());
	// }

	// Debugger::add_cycle_event("Freeing alias", {
	// 	{ "Architectural register", to_string(id) },
	// 	{ "Free registers", {
	// 		{ "Empty", free_aliases_empty.to_bool() },
	// 		{ "Full",  free_aliases_full.to_bool()  },
	// 		{ "List",  array                        }
	// 	} }
	// });
	// #endif // __SYNTHESIS__
}

#ifdef DBG_SYNTH
void DecodeStage::interface(const FetchToDecode& from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, DecodeToCommit* to_commit, DecodeStatus* status) {
#else
void DecodeStage::interface(const FetchToDecode& from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, DecodeToCommit* to_commit) {
#endif
	#pragma HLS inline
	#pragma array_partition variable=register_map complete
	#pragma array_partition variable=free_aliases complete

	// TODO : update ready registers from write back and commit
	// TODO : unmap unused registers, except if it is zero

	bit_t do_smth = from_fetch.has_fetched;

	if (do_smth) {
		const opcode_prefix_t      opcode_prefix = from_fetch.instruction(Opcode::Prefix::high      , Opcode::Prefix::low);
		const opcode_suffix_low_t  opcode_low    = from_fetch.instruction(Opcode::Suffix::Low::high , Opcode::Suffix::Low::low);
		const opcode_suffix_high_t opcode_high   = from_fetch.instruction(Opcode::Suffix::High::high, Opcode::Suffix::High::low);

		const reg_t dest = from_fetch.instruction(Dest::high, Dest::low);
		const reg_t src1 = from_fetch.instruction(Src1::high, Src1::low);
		const reg_t src2 = from_fetch.instruction(Src2::high, Src2::low);

		const func3_t func3 = from_fetch.instruction(Func3::high, Func3::low);
		const func7_t func7 = from_fetch.instruction(Func7::high, Func7::low);

		const bool is_func7_0b0x0000x = !(func7 & 0b1011110);

		Type type                    = Architecture::Type::unknown_type; // Default instruction type
		InstructionCategory category = Architecture::InstructionCategory::system; // Default instruction category

		bit_t invalid_instruction = false;

		bit_t is_jmp               = false;
		bit_t is_jmp_address_known = false;

		if (opcode_prefix == 0b11) {
			switch (opcode_high) {
			case Opcode::Suffix::High::load_fence_auipc:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h00_load : type     = Architecture::Type::I;
				                                     category = Architecture::InstructionCategory::load;  break;
				case Opcode::Suffix::Low::h00_001  :                                                      break;
				case Opcode::Suffix::Low::h00_010  :                                                      break;
				case Opcode::Suffix::Low::h00_fence: type     = Architecture::Type::I;
				                                     category = Architecture::InstructionCategory::fence;
				                                     invalid_instruction = func3 != 0b000;                break;
				case Opcode::Suffix::Low::h00_alui : type     = Architecture::Type::I;
				                                     category = Architecture::InstructionCategory::alui;  break;
				case Opcode::Suffix::Low::h00_auipc: type     = Architecture::Type::U;
				                                     category = Architecture::InstructionCategory::auipc; break;
				case Opcode::Suffix::Low::h00_110  :                                                      break;
				case Opcode::Suffix::Low::h00_111  :                                                      break;
				}
				break;
			case Opcode::Suffix::High::store_alu_lui:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h01_store: type     = Architecture::Type::S;
				                                     category = Architecture::InstructionCategory::store; break;
				case Opcode::Suffix::Low::h01_001  :                                                      break;
				case Opcode::Suffix::Low::h01_010  :                                                      break;
				case Opcode::Suffix::Low::h01_011  :                                                      break;
				case Opcode::Suffix::Low::h01_alu  : type     = Architecture::Type::R;
				                                     category = Architecture::InstructionCategory::alu;   break;
				case Opcode::Suffix::Low::h01_lui  : type     = Architecture::Type::U;
				                                     category = Architecture::InstructionCategory::lui;   break;
				case Opcode::Suffix::Low::h01_110  :                                                      break;
				case Opcode::Suffix::Low::h01_111  :                                                      break;
				}
				break;
			case Opcode::Suffix::High::unused_10:
				break;
			case Opcode::Suffix::High::branch_jal_r_system:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h11_branch: type = Architecture::Type::B;
				                                      category = Architecture::InstructionCategory::branch;
				                                      is_jmp = true;                                        break;
				case Opcode::Suffix::Low::h11_jalr  : type = Architecture::Type::I;
				                                      category = Architecture::InstructionCategory::jalr;
				                                      is_jmp = true;                                        break;
				case Opcode::Suffix::Low::h11_010   :                                                       break;
				case Opcode::Suffix::Low::h11_jal   : type = Architecture::Type::J;
				                                      category = Architecture::InstructionCategory::jal;
				                                      is_jmp = true;
				                                      is_jmp_address_known = true;                          break;
				case Opcode::Suffix::Low::h11_system: type = Architecture::Type::I;
				                                      category = Architecture::InstructionCategory::system;
				                                      invalid_instruction =    dest != Architecture::Register::x0
													                        || src1 != Architecture::Register::x0;
													                                                        break;
				case Opcode::Suffix::Low::h11_101   :                                                       break;
				case Opcode::Suffix::Low::h11_110   :                                                       break;
				case Opcode::Suffix::Low::h11_111   :                                                       break;
				}
				break;
			}
		}

		packed_immediate_t packed_immediate;
		program_counter_t  pc_offset;

		bit_t use_dest = false;
		bit_t use_src1 = false;
		bit_t use_src2 = false;

		// TODO : performances : plutôt qu'utiliser des enums, utiliser des flags ?
		switch (type) {
		case Architecture::Type::R:
			use_src1 = true;
			use_src2 = true;
			use_dest = true;
			break;
		case Architecture::Type::I:
			packed_immediate = pack_I_immediate(from_fetch.instruction);
			use_src1 = true;
			use_src2 = false;
			use_dest = true;
			break;
		case Architecture::Type::S:
			packed_immediate = pack_S_immediate(from_fetch.instruction);
			use_src1 = true;
			use_src2 = true;
			use_dest = false;
			break;
		case Architecture::Type::B:
			packed_immediate = pack_B_immediate(from_fetch.instruction);
			use_src1 = true;
			use_src2 = true;
			use_dest = false;
			break;
		case Architecture::Type::U:
			packed_immediate = pack_U_immediate(from_fetch.instruction);
			use_src1 = false;
			use_src2 = false;
			use_dest = true;
			break;
		case Architecture::Type::J:
			pc_offset = extract_J_pc_offset(from_fetch.instruction);
			use_src1 = false;
			use_src2 = false;
			use_dest = true;
			// the JAL instruction is invalid if the immediate is not aligned to a four-byte boundary
			invalid_instruction = from_fetch.instruction.get_bit(J::InInstruction::Imm_1::pos);
			break;
		case Architecture::Type::unknown_type:
			invalid_instruction = true;
			break;
		}

		// Sending all informations to other stages

		if (is_jmp) {
			if (is_jmp_address_known) {
				to_fetch->has_next_pc = !invalid_instruction;
				to_fetch->next_pc     = from_fetch.pc + pc_offset;
			}
			else {
				to_fetch->has_next_pc = false;
			}
		}
		else {
			to_fetch->has_next_pc = true;
			to_fetch->next_pc     = from_fetch.pc + 1;
		}

		bit_t blocked_register_map = false;
		physical_reg_t physical_dest;
		physical_reg_t physical_src1;
		physical_reg_t physical_src2;
		if (use_src1) get_register_alias(src1, &physical_src1);
		if (use_src2) get_register_alias(src2, &physical_src2);
		if (use_dest) {
			if (dest.is_zero())
				physical_dest = 0;
			else
				create_register_alias(dest, &physical_dest, &blocked_register_map); // TODO : smth with blocking : block previous stages, add inter-stage registers to hold temporary results ?
		}

		to_issue->has_decoded_instruction = !invalid_instruction; // Only add the instruction to the issue table if it is a valid instruction
		to_issue->token                   = token;
		to_issue->pc                      = from_fetch.pc;
		to_issue->category                = category;
		to_issue->func3                   = func3;
		to_issue->is_func7_0b0000000      = is_func7_0b0x0000x && !func7.test(5) && !func7.test(0);
		to_issue->is_func7_0b0000001      = is_func7_0b0x0000x && !func7.test(5) &&  func7.test(0);
		to_issue->is_func7_0b0100000      = is_func7_0b0x0000x &&  func7.test(5) && !func7.test(0);
		to_issue->packed_immediate        = packed_immediate;
		to_issue->use_dest                = use_dest;
		to_issue->use_src1                = use_src1;
		to_issue->use_src2                = use_src2;
		to_issue->dest                    = physical_dest;
		to_issue->src1                    = physical_src1;
		to_issue->src2                    = physical_src2;
		to_issue->src1_ready              = ready_registers.get_bit(physical_src1);
		to_issue->src2_ready              = ready_registers.get_bit(physical_src2);

		to_commit->add_to_rob = true;
		to_commit->token      = token;
		to_commit->invalid    = invalid_instruction;
		token++;
	}
	else {
		to_fetch->has_next_pc             = false;
		to_issue->has_decoded_instruction = false;
		to_commit->add_to_rob             = false;
	}

	#ifndef __SYNTHESIS__
	json json_register_map = json::object();
	for (uint16_t i = 0; i < architectural_register_count; i++) {
		json_register_map.push_back({ std::to_string(i), register_map[i].to_uint() });
	}

	json json_free_aliases = json::array();
	FOR_CYCLE_BUFFER(free_aliases, physical_register_count) {
		json_free_aliases.push_back(free_aliases[i].to_uint());
	}

	Debugger::add_cycle_event("Decode stage", {
		{ "Register map",       json_register_map                                 },
		{ "Ready registers",    string_bin<64>(ready_registers.to_uint64(), true) },
		{ "Free aliases",       json_free_aliases                                 },
		{ "Free aliases bot",   free_aliases_bot.to_uint()                        },
		{ "Free aliases top",   free_aliases_top.to_uint()                        },
		{ "Free aliases empty", free_aliases_empty.to_bool()                      },
		{ "Free aliases full",  free_aliases_full.to_bool()                       }
	});
	#endif // __SYNTHESIS__

	#ifdef DBG_SYNTH
	for (uint16_t i = 0; i < architectural_register_count; i++)
		status->register_map[i] = register_map[i];
	status->ready_registers    = ready_registers;
	for (uint16_t i = 0; i < physical_register_count; i++)
		status->free_aliases[i] = free_aliases[i];
	status->free_aliases_bot    = free_aliases_bot;
	status->free_aliases_top    = free_aliases_top;
	status->free_aliases_empty  = free_aliases_empty;
	status->free_aliases_full   = free_aliases_full;
	status->did_smth            = do_smth;
	#endif
}
