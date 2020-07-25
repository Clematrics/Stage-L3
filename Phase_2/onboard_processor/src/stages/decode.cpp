/* ****************************************************************************
*    Source file for the decode stage
**************************************************************************** */

#include "stages/decode.hpp"

#include "architecture/decoding.hpp"
#include "architecture/immediate_packing.hpp"
#include "architecture/architecture_helpers.hpp"
#include "debug/debug.hpp"

using namespace Architecture::Type;
using namespace Decoding;

DecodeStage::DecodeStage() {
	token = initial_token;

	// Initialization of the register map
	for (uint16_t id = 0; id < architectural_register_count; id++) {
		#pragma HLS unroll
		register_map[id] = id;
	}

	// Initialization of the free aliases
	for (uint16_t id = architectural_register_count; id < physical_register_count; id++) {
		#pragma HLS unroll
		free_aliases[id - architectural_register_count] = id;
		free_aliases_empty = false;
	}
	free_aliases_top = physical_register_count - architectural_register_count;
	if (free_aliases_bot == free_aliases_top)
		free_aliases_full = true;
}

void DecodeStage::free_aliases_push_back(physical_reg_t entry) {
	#pragma HLS inline

	free_aliases[free_aliases_top] = entry;
	free_aliases_top++;
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

void DecodeStage::get_register_alias(const reg_t& id, physical_reg_t* alias) {
	#pragma HLS inline

	*alias = register_map[id];
}

void DecodeStage::create_register_alias(const reg_t& id, physical_reg_t* alias, bit_t* blocking) {
	#pragma HLS inline

	if (free_aliases_empty) {
		*blocking = true;
	}
	else {
		physical_reg_t new_alias = free_aliases[free_aliases_bot];
		free_aliases_pop();
		*alias = register_map[id] = new_alias;
		*blocking = free_aliases_empty;
	}

	#ifndef __SYNTHESIS__
	json json_map = json::object();
	for (uint16_t id = 0 ; id < architectural_register_count ; id++)
		json_map.push_back({ to_string(static_cast<reg_t>(id)), register_map[id].to_uint() });
	json array = json::array();
	for (uint32_t i = free_aliases_bot, do_loop = !free_aliases_empty ; i != free_aliases_top || do_loop ; i = (i + 1) % physical_register_count, do_loop = false)
		array.push_back(free_aliases[i].to_uint());
	Debugger::add_cycle_event({
		{ "Creating alias",
			{
				{ "Architectural register",    to_string(id)    },
				{ "Physical register created", alias->to_uint() },
				{ "Current mapping",           json_map         },
				{ "Free registers", {
					{ "Empty", free_aliases_empty.to_bool() },
					{ "Full",  free_aliases_full.to_bool()  },
					{ "List",  array                        }
				} }
			}
		}
	});
	#endif // __SYNTHESIS__
}

void DecodeStage::free_register_alias(const physical_reg_t& id) {
	#pragma HLS inline
	free_aliases_push_back(id);

	#ifndef __SYNTHESIS__
	json array = json::array();
	for (uint32_t i = free_aliases_bot, do_loop = !free_aliases_empty ; i != free_aliases_top || do_loop ; i = (i + 1) % physical_register_count, do_loop = false)
		array.push_back(free_aliases[i].to_uint());
	Debugger::add_cycle_event({
		{ "Freeing alias",
			{
				{ "Architectural register", to_string(id) },
				{ "Free registers", {
					{ "Empty", free_aliases_empty.to_bool() },
					{ "Full",  free_aliases_full.to_bool()  },
					{ "List",  array                        }
				} }
			}
		}
	});
	#endif // __SYNTHESIS__
}

void DecodeStage::interface(FetchToDecode& from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, DecodeToCommit* to_commit, bit_t* decode_ran) {
	#pragma HLS inline
	#pragma array_partition variable=register_map complete
	#pragma array_partition variable=free_aliases complete

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

		Type type = Architecture::Type::unknown_type; // Default instruction type

		bit_t is_jmp               = false;
		bit_t is_jmp_address_known = false;

		if (opcode_prefix == 0b11) {
			switch (opcode_high) {
			case Opcode::Suffix::High::load_fence_auipc:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h00_load : type = Architecture::Type::I;  break;
				case Opcode::Suffix::Low::h00_001  :                                break;
				case Opcode::Suffix::Low::h00_010  :                                break;
				case Opcode::Suffix::Low::h00_fence: type = Architecture::Type::I;  break;
				case Opcode::Suffix::Low::h00_alui : type = Architecture::Type::I;  break;
				case Opcode::Suffix::Low::h00_auipc: type = Architecture::Type::U;  break;
				case Opcode::Suffix::Low::h00_110  :                                break;
				case Opcode::Suffix::Low::h00_111  :                                break;
				}
				break;
			case Opcode::Suffix::High::store_alu_lui:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h01_store: type = Architecture::Type::S;  break;
				case Opcode::Suffix::Low::h01_001  :                                break;
				case Opcode::Suffix::Low::h01_010  :                                break;
				case Opcode::Suffix::Low::h01_011  :                                break;
				case Opcode::Suffix::Low::h01_alu  : type = Architecture::Type::R;  break;
				case Opcode::Suffix::Low::h01_lui  : type = Architecture::Type::U;  break;
				case Opcode::Suffix::Low::h01_110  :                                break;
				case Opcode::Suffix::Low::h01_111  :                                break;
				}
				break;
			case Opcode::Suffix::High::unused_10:
				break;
			case Opcode::Suffix::High::branch_jal_r_system:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h11_branch: type = Architecture::Type::B;
				                                      is_jmp = true;                break;
				case Opcode::Suffix::Low::h11_jalr  : type = Architecture::Type::I;
				                                      is_jmp = true;                break;
				case Opcode::Suffix::Low::h11_010   :                               break;
				case Opcode::Suffix::Low::h11_jal   : type = Architecture::Type::J;
				                                      is_jmp = true;
				                                      is_jmp_address_known = true;  break;
				case Opcode::Suffix::Low::h11_system: type = Architecture::Type::I; break;
				case Opcode::Suffix::Low::h11_101   :                               break;
				case Opcode::Suffix::Low::h11_110   :                               break;
				case Opcode::Suffix::Low::h11_111   :                               break;
				}
				break;
			}
		}

		packed_immediate_t packed_immediate;
		program_counter_t  pc_offset;
		bit_t              invalid_instruction = false;

		bit_t use_dest = false;
		bit_t use_src1 = false;
		bit_t use_src2 = false;

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
		to_issue->has_decoded_instruction = true;
		to_issue->pc                      = from_fetch.pc;
		to_issue->func3                   = func3;
		to_issue->is_func7_0b0000000      = is_func7_0b0x0000x && !func7.test(5) && !func7.test(0);
		to_issue->is_func7_0b0000001      = is_func7_0b0x0000x && !func7.test(5) &&  func7.test(0);
		to_issue->is_func7_0b0100000      = is_func7_0b0x0000x &&  func7.test(5) && !func7.test(0);
		to_issue->packed_immediate        = packed_immediate;
		to_issue->use_dest                = use_dest;
		to_issue->use_src1                = use_src1;
		to_issue->use_src2                = use_src2;
		if (use_src1) get_register_alias(src1, &to_issue->src1);
		if (use_src2) get_register_alias(src2, &to_issue->src2);
		if (use_dest) create_register_alias(dest, &to_issue->dest, &blocked_register_map); // TODO : smth with blocking : block previous stages, add inter-stage registers to hold temporary results ?
		to_issue->invalid                 = invalid_instruction;

		to_commit->add_to_rob = true;
		to_commit->token      = token;
		to_commit->invalid    = invalid_instruction;
		token++;

		#ifndef __SYNTHESIS__
		Debugger::add_cycle_event({
			{ "Decode stage",
				{
					{ "To fetch",
						{
							{ "Has a next pc", to_fetch->has_next_pc.to_bool() },
							{ "Next pc",       to_fetch->next_pc.to_uint()     }
						}
					},
					{ "To issue",
						{
							{ "Has decoded an instruction", to_issue->has_decoded_instruction.to_bool()                  },
							{ "Program counter",            to_issue->pc.to_uint()                                       },
							{ "Func 3",                     to_issue->func3.to_uint()                                    },
							{ "Is Func7 0b0000000",         to_issue->is_func7_0b0000000.to_bool()                       },
							{ "Is Func7 0b0000001",         to_issue->is_func7_0b0000001.to_bool()                       },
							{ "Is Func7 0b0100000",         to_issue->is_func7_0b0100000.to_bool()                       },
							{ "Packed immediate",           unpack_immediate(type, to_issue->packed_immediate).to_uint() },
							{ "Use destination",            to_issue->use_dest.to_bool()                                 },
							{ "Use source 1",               to_issue->use_src1.to_bool()                                 },
							{ "Use source 2",               to_issue->use_src2.to_bool()                                 },
							{ "Physical destination",       to_issue->dest.to_uint()                                     },
							{ "Physical source 1",          to_issue->src1.to_uint()                                     },
							{ "Physical source 2",          to_issue->src2.to_uint()                                     },
							{ "Invalid instruction",        to_issue->invalid.to_bool()                                  }
						}
					},
					{ "To commit",
						{
							{ "Add instruction to ROB", to_commit->add_to_rob.to_bool() },
							{ "Token",                  to_commit->token.to_uint()      },
							{ "Invalid instruction",    to_commit->invalid.to_bool()    }
						}
					}
				}
			}
		});
		#endif // __SYNTHESIS__
	}
	else {
		to_fetch->has_next_pc             = false;
		to_issue->has_decoded_instruction = false;
		to_commit->add_to_rob             = false;
	}
	*decode_ran = do_smth;
}
