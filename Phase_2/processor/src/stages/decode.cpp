#include "stages/decode.hpp"

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"
#include "architecture/disasm.hpp"
#include "architecture/immediate_packing.hpp"

using namespace Decoding;

DecodeStage::DecodeStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active),
	  token(0)
{
}

void DecodeStage::interface(const FetchToDecode& from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, DecodeToCommit* to_commit, bool* stop) {
	#pragma HLS INLINE

	if (is_active()) {
		if (!from_fetch.has_fetched_instruction) {
			to_fetch->block_fetch_stage = true; // TODO : check if this can create a deadlock
			to_issue->has_decoded_instruction = false;
			return;
		}

		const opcode_prefix_t      opcode_prefix = from_fetch.instruction(Opcode::Prefix::high      , Opcode::Prefix::low);
		const opcode_suffix_low_t  opcode_low    = from_fetch.instruction(Opcode::Suffix::Low::high , Opcode::Suffix::Low::low);
		const opcode_suffix_high_t opcode_high   = from_fetch.instruction(Opcode::Suffix::High::high, Opcode::Suffix::High::low);

		const reg_t dest = from_fetch.instruction(Dest::high, Dest::low);
		const reg_t src1 = from_fetch.instruction(Src1::high, Src1::low);
		const reg_t src2 = from_fetch.instruction(Src2::high, Src2::low);

		const func3_t func3 = from_fetch.instruction(Func3::high, Func3::low);
		const func7_t func7 = from_fetch.instruction(Func7::high, Func7::low);
		to_issue->func3 = func3;

		const bool is_func7_0b0x0000x = !(func7 & 0b1011110);
		to_issue-> is_func7_0b0000000 = is_func7_0b0x0000x && !func7.test(5) && !func7.test(0);
		to_issue-> is_func7_0b0000001 = is_func7_0b0x0000x && !func7.test(5) &&  func7.test(0);
		to_issue-> is_func7_0b0100000 = is_func7_0b0x0000x &&  func7.test(5) && !func7.test(0);

		to_issue->program_counter = from_fetch.program_counter;

		to_issue->type = Architecture::Type::unknown_type;
		to_issue->kind = Decoding::Kind::unknown_kind;

		bool is_jump_instruction = false;

		if (opcode_prefix == 0b11) {
			switch (opcode_high) {
			case Opcode::Suffix::High::load_fence_auipc:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h00_load : to_issue->type = Architecture::Type::I; to_issue->kind = Decoding::Kind::load;                             break;
				case Opcode::Suffix::Low::h00_001  :                                                                                                            break;
				case Opcode::Suffix::Low::h00_010  :                                                                                                            break;
				case Opcode::Suffix::Low::h00_fence: to_issue->type = Architecture::Type::I; to_issue->kind = Decoding::Kind::fence;                            break;
				case Opcode::Suffix::Low::h00_alui : to_issue->type = Architecture::Type::I; to_issue->kind = Decoding::Kind::alu_immediate;                    break;
				case Opcode::Suffix::Low::h00_auipc: to_issue->type = Architecture::Type::U; to_issue->kind = Decoding::Kind::auipc;                            break;
				case Opcode::Suffix::Low::h00_110  :                                                                                                            break;
				case Opcode::Suffix::Low::h00_111  :                                                                                                            break;
				}
				break;
			case Opcode::Suffix::High::store_alu_lui:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h01_store: to_issue->type = Architecture::Type::S; to_issue->kind = Decoding::Kind::store;                            break;
				case Opcode::Suffix::Low::h01_001  :                                                                                                            break;
				case Opcode::Suffix::Low::h01_010  :                                                                                                            break;
				case Opcode::Suffix::Low::h01_011  :                                                                                                            break;
				case Opcode::Suffix::Low::h01_alu  : to_issue->type = Architecture::Type::R; to_issue->kind = Decoding::Kind::alu;                              break;
				case Opcode::Suffix::Low::h01_lui  : to_issue->type = Architecture::Type::U; to_issue->kind = Decoding::Kind::lui;                              break;
				case Opcode::Suffix::Low::h01_110  :                                                                                                            break;
				case Opcode::Suffix::Low::h01_111  :                                                                                                            break;
				}
				break;
			case Opcode::Suffix::High::h10_unused:
				break;
			case Opcode::Suffix::High::branch_jal_r_system:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h11_branch: to_issue->type = Architecture::Type::B; to_issue->kind = Decoding::Kind::branch;                          break;
				case Opcode::Suffix::Low::h11_jalr  : to_issue->type = Architecture::Type::I; to_issue->kind = Decoding::Kind::jalr;                            break;
				case Opcode::Suffix::Low::h11_010   :                                                                                                           break;
				case Opcode::Suffix::Low::h11_jal   : to_issue->type = Architecture::Type::J; to_issue->kind = Decoding::Kind::jal; is_jump_instruction = true; break;
				case Opcode::Suffix::Low::h11_system: to_issue->type = Architecture::Type::I; to_issue->kind = Decoding::Kind::system;                          break;
				case Opcode::Suffix::Low::h11_101   :                                                                                                           break;
				case Opcode::Suffix::Low::h11_110   :                                                                                                           break;
				case Opcode::Suffix::Low::h11_111   :                                                                                                           break;
				}
				break;
			}
		}

		switch (to_issue->type) {
		case Architecture::Type::R:
			to_issue->use_src1 = true;
			to_issue->use_src2 = true;
			to_issue->use_dest = true;
			break;
		case Architecture::Type::I:
			to_issue->packed_immediate = pack_I_immediate(from_fetch.instruction);
			to_issue->use_src1 = true;
			to_issue->use_src2 = false;
			to_issue->use_dest = true;
			break;
		case Architecture::Type::S:
			to_issue->packed_immediate = pack_S_immediate(from_fetch.instruction);
			to_issue->use_src1 = true;
			to_issue->use_src2 = true;
			to_issue->use_dest = false;
			break;
		case Architecture::Type::B:
			to_issue->packed_immediate = pack_B_immediate(from_fetch.instruction);
			to_issue->use_src1 = true;
			to_issue->use_src2 = true;
			to_issue->use_dest = false;
			break;
		case Architecture::Type::U:
			to_issue->packed_immediate = pack_U_immediate(from_fetch.instruction);
			to_issue->use_src1 = false;
			to_issue->use_src2 = false;
			to_issue->use_dest = true;
			break;
		case Architecture::Type::J:
			to_issue->packed_immediate = pack_J_immediate(from_fetch.instruction);
			to_issue->use_src1 = false;
			to_issue->use_src2 = false;
			to_issue->use_dest = true;
			break;
		case Architecture::Type::unknown_type:
			break;
		}

		bool blocking = false;
		if (to_issue->use_src1)
			register_map.get_alias(dest, &to_issue->src1);
		if (to_issue->use_src2)
			register_map.get_alias(dest, &to_issue->src2);
		if (to_issue->use_dest)
			register_map.create_alias(dest, &to_issue->dest, &blocking);
		// TODO : make something with blocking

		// TODO : transfer informations to commit for the ROB
		to_commit->has_decoded_instruction = true;
		to_commit->token = token;
		to_issue->token = token;
		token++;

		if (to_issue->type == Architecture::Type::unknown_type) {
			*stop = true;
		}

		if (is_jump_instruction) {
			program_counter_t offset;
			offset(J::InUnpackedImmediate::Offset::high,              J::InUnpackedImmediate::Offset::low)              = to_issue->packed_immediate(J::InPackedImmediate::JumpOffset::high, J::InPackedImmediate::JumpOffset::low);
			offset(J::InUnpackedImmediate::OffsetSignExtension::high, J::InUnpackedImmediate::OffsetSignExtension::low) = to_issue->packed_immediate.test(J::InPackedImmediate::Sign::pos) ? -1 : 0;
			to_fetch->next_program_counter = from_fetch.program_counter + offset;
		}
		else {
			to_fetch->next_program_counter = from_fetch.program_counter + 1;
		}

		#ifndef __SYNTHESIS__
		opcode_suffix_t opcode_suffix = from_fetch.instruction(Opcode::Suffix::high, Opcode::Suffix::low);
		opcode_t opcode               = from_fetch.instruction(Opcode::high, Opcode::low);
		DisassembledInstruction instr = {
			from_fetch.instruction,
			opcode,
			opcode_prefix,
			opcode_suffix,
			func3,
			func7,
			get_instruction_name(from_fetch.instruction, opcode_prefix, opcode_high, opcode_low, func3, func7),
			to_issue->type,
			dest,
			src1,
			src2,
			unpack_immediate(to_issue->type, to_issue->packed_immediate)
		};
		Debugger::add_cycle_event({
			{ "Decode stage",
				{
					{ "Decoded instruction", instruction_to_json(instr) },
					{ "Next program counter", to_fetch->next_program_counter.to_uint() },
					{ "Is jump instruction ?", is_jump_instruction },
					{ "Is unknown instruction ?", to_issue->type == Architecture::Type::unknown_type }
				}
			}
		});
		#endif // __SYNTHESIS__
	}
	tick();
}