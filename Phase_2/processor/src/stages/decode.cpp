#include "stages/decode.hpp"

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"
#include "architecture/disasm.hpp"

using namespace Decoding;

DecodeStage::DecodeStage(uint16_t cycles_activity_period, uint16_t cycles_before_active)
	: Stage(cycles_activity_period, cycles_before_active)
{
}

Name get_instruction_name(const word_t& instruction, const opcode_prefix_t& opcode_prefix, const opcode_suffix_high_t& opcode_high, const opcode_suffix_low_t& opcode_low, const func3_t& func3, const func7_t& func7) {
	const bool is_func7_0b0x0000x = !(func7 & 0b1011110);
	const bool is_func7_0b0000000 = is_func7_0b0x0000x && !func7.test(5) && !func7.test(0);
	const bool is_func7_0b0000001 = is_func7_0b0x0000x && !func7.test(5) &&  func7.test(0);
	const bool is_func7_0b0100000 = is_func7_0b0x0000x &&  func7.test(5) && !func7.test(0);

	if (opcode_prefix == 0b11) {
		switch (opcode_high) {
		case Opcode::Suffix::High::load_fence_auipc:
			switch (opcode_low) {
			case Opcode::Suffix::Low::h00_load:
				switch (func3) {
				case Func3::lb:  return Name::LB;
				case Func3::lh:  return Name::LH;
				case Func3::lw:  return Name::LW;
				case Func3::lbu: return Name::LBU;
				case Func3::lhu: return Name::LHU;
				default:         return Name::unknown;
				}
			case Opcode::Suffix::Low::h00_001:
				return Name::unknown;
			case Opcode::Suffix::Low::h00_010:
				return Name::unknown;
			case Opcode::Suffix::Low::h00_fence:
				if (func3 == Func3::fence) return Name::FENCE;
				else                       return Name::unknown;
			case Opcode::Suffix::Low::h00_alui:
				switch (func3) {
				case Func3::addi:                return Name::ADDI;
				case Func3::slli:
					if      (is_func7_0b0000000) return Name::SLLI;
					else                         return Name::unknown;
				case Func3::slti:                return Name::SLTI;
				case Func3::sltiu:               return Name::SLTIU;
				case Func3::xori:                return Name::XORI;
				case Func3::srli_srai:
					if      (is_func7_0b0000000) return Name::SRLI;
					else if (is_func7_0b0100000) return Name::SRAI;
					else                         return Name::unknown;
				case Func3::ori:                 return Name::ORI;
				case Func3::andi:                return Name::ANDI;
				}
			case Opcode::Suffix::Low::h00_auipc:
				return Name::AUIPC;
			case Opcode::Suffix::Low::h00_110:
				return Name::unknown;
			case Opcode::Suffix::Low::h00_111:
				return Name::unknown;
			}
		case Opcode::Suffix::High::store_alu_lui:
			switch (opcode_low) {
			case Opcode::Suffix::Low::h01_store:
				if (func3.test(2)) {
					return Name::unknown;
				} else {
					switch (func3(1, 0)) {
					case 0b00: return Name::SB;
					case 0b01: return Name::SH;
					case 0b10: return Name::SW;
					case 0b11: return Name::unknown;
					}
				}
			case Opcode::Suffix::Low::h01_001:
				return Name::unknown;
			case Opcode::Suffix::Low::h01_010:
				return Name::unknown;
			case Opcode::Suffix::Low::h01_011:
				return Name::unknown;
			case Opcode::Suffix::Low::h01_alu:
				switch (func3) {
				case Func3::add_sub_mul:
					if      (is_func7_0b0000000) return Name::ADD;
					else if (is_func7_0b0100000) return Name::SUB;
					else if (is_func7_0b0000001) return Name::MUL;
					else                         return Name::unknown;
				case Func3::sll_mulh:
					if      (is_func7_0b0000000) return Name::SLL;
					else if (is_func7_0b0000001) return Name::MULH;
					else                         return Name::unknown;
				case Func3::slt_mulhsu:
					if      (is_func7_0b0000000) return Name::SLT;
					else if (is_func7_0b0000001) return Name::MULHSU;
					else                         return Name::unknown;
				case Func3::sltu_mulhu:
					if      (is_func7_0b0000000) return Name::SLTU;
					else if (is_func7_0b0000001) return Name::MULHU;
					else                         return Name::unknown;
				case Func3::xor_div:
					if      (is_func7_0b0000000) return Name::XOR;
					else if (is_func7_0b0000001) return Name::DIV;
					else                         return Name::unknown;
				case Func3::srl_sra_divu:
					if      (is_func7_0b0000000) return Name::SRL;
					else if (is_func7_0b0100000) return Name::SRA;
					else if (is_func7_0b0000001) return Name::DIVU;
					else                         return Name::unknown;
				case Func3::or_rem:
					if      (is_func7_0b0000000) return Name::OR;
					else if (is_func7_0b0000001) return Name::REM;
					else                         return Name::unknown;
				case Func3::and_remu:
					if      (is_func7_0b0000000) return Name::AND;
					else if (is_func7_0b0000001) return Name::REMU;
					else                         return Name::unknown;
				}
			case Opcode::Suffix::Low::h01_lui:
				return Name::LUI;
			case Opcode::Suffix::Low::h01_110:
				return Name::unknown;
			case Opcode::Suffix::Low::h01_111:
				return Name::unknown;
			}
		case Opcode::Suffix::High::h10_unused:
			return Name::unknown;
		case Opcode::Suffix::High::branch_jal_r_system:
			switch (opcode_low) {
			case Opcode::Suffix::Low::h11_branch:
				switch (func3) {
				case Func3::beq:  return Name::BEQ;
				case Func3::bne:  return Name::BNE;
				case Func3::blt:  return Name::BLT;
				case Func3::bge:  return Name::BGE;
				case Func3::bltu: return Name::BLTU;
				case Func3::bgeu: return Name::BGEU;
				default:          return Name::unknown;
				}
			case Opcode::Suffix::Low::h11_jalr:
				if (func3 == Func3::jalr) {
					return Name::JALR;
				}
				else {
					return Name::unknown;
				}
			case Opcode::Suffix::Low::h11_010:
				return Name::unknown;
			case Opcode::Suffix::Low::h11_jal:
				return Name::JAL;
			case Opcode::Suffix::Low::h11_system: {
				const ap_uint<25> no_opcode = instruction(31, 7);
				if      (no_opcode == 0b0000000000000000000000000) return Name::EBREAK;
				else if (no_opcode == 0b0000000000010000000000000) return Name::ECALL;
				else                                               return Name::unknown;
				}
			case Opcode::Suffix::Low::h11_101:
				return Name::unknown;
			case Opcode::Suffix::Low::h11_110:
				return Name::unknown;
			case Opcode::Suffix::Low::h11_111:
				return Name::unknown;
			}
		}
	} else {
		return Name::unknown;
	}
}

packed_immediate_t pack_I_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(I::InPackedImmediate::Imm_11_0::high, I::InPackedImmediate::Imm_11_0::low) = instruction(I::InInstruction::Imm_11_0::high, I::InInstruction::Imm_11_0::low);
	return immediate;
}

packed_immediate_t pack_S_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(S::InPackedImmediate::Imm_4_0::high,  S::InPackedImmediate::Imm_4_0::low)  = instruction(S::InInstruction::Imm_4_0::high , S::InInstruction::Imm_4_0::low);
	immediate(S::InPackedImmediate::Imm_11_5::high, S::InPackedImmediate::Imm_11_5::low) = instruction(S::InInstruction::Imm_11_5::high, S::InInstruction::Imm_11_5::low);
	return immediate;
}

packed_immediate_t pack_B_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(B::InPackedImmediate::Imm_4_1::high,  B::InPackedImmediate::Imm_4_1::low)  = instruction(B::InInstruction::Imm_4_1::high , B::InInstruction::Imm_4_1::low);
	immediate(B::InPackedImmediate::Imm_10_5::high, B::InPackedImmediate::Imm_10_5::low) = instruction(B::InInstruction::Imm_10_5::high, B::InInstruction::Imm_10_5::low);
	immediate.set_bit(B::InPackedImmediate::Imm_11::pos, instruction.get_bit(B::InInstruction::Imm_11::pos));
	immediate.set_bit(B::InPackedImmediate::Imm_12::pos, instruction.get_bit(B::InInstruction::Imm_12::pos));
	return immediate;
}

packed_immediate_t pack_U_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(U::InPackedImmediate::Imm_31_12::high, U::InPackedImmediate::Imm_31_12::low) = instruction(U::InInstruction::Imm_31_12::high, U::InInstruction::Imm_31_12::low);
	return immediate;
}

packed_immediate_t pack_J_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(J::InPackedImmediate::Imm_10_1::high, J::InPackedImmediate::Imm_10_1::low)   = instruction(J::InInstruction::Imm_10_1::high, J::InInstruction::Imm_10_1::low);
	immediate.set_bit(J::InPackedImmediate::Imm_11::pos, instruction.get_bit(J::InInstruction::Imm_11::pos));
	immediate(J::InPackedImmediate::Imm_19_12::high, J::InPackedImmediate::Imm_19_12::low) = instruction(J::InInstruction::Imm_19_12::high, J::InInstruction::Imm_19_12::low);
	immediate.set_bit(J::InPackedImmediate::Imm_20::pos, instruction.get_bit(J::InInstruction::Imm_20::pos));
	return immediate;
}

word_t unpack_I_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(11,  0) = packed_immediate(I::InPackedImmediate::Imm_11_0::high, I::InPackedImmediate::Imm_11_0::low);
	immediate(31, 12) = packed_immediate.get_bit(I::InPackedImmediate::Sign::pos) ? -1 : 0;
	return immediate;
}

word_t unpack_S_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(S::InUnpackedImmediate::Imm_11_0::high, S::InUnpackedImmediate::Imm_11_0::low) = packed_immediate(S::InPackedImmediate::Imm_11_0::high, S::InPackedImmediate::Imm_11_0::low);
	immediate(S::InUnpackedImmediate::SignExtension::high, S::InUnpackedImmediate::SignExtension::low) = packed_immediate.get_bit(S::InPackedImmediate::Sign::pos) ? -1 : 0;
	return immediate;
}

word_t unpack_B_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate.set_bit(B::InUnpackedImmediate::Imm_0::pos, B::InUnpackedImmediate::Imm_0::value);
	immediate(B::InUnpackedImmediate::Imm_12_1::high, B::InUnpackedImmediate::Imm_12_1::low) = packed_immediate(B::InPackedImmediate::Imm_12_1::high, B::InPackedImmediate::Imm_12_1::low);
	immediate(B::InUnpackedImmediate::SignExtension::high, B::InUnpackedImmediate::SignExtension::low) = packed_immediate.get_bit(S::InPackedImmediate::Sign::pos) ? -1 : 0;
	return immediate;
}

word_t unpack_U_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(U::InUnpackedImmediate::Imm_11_0::high, U::InUnpackedImmediate::Imm_11_0::low) = U::InUnpackedImmediate::Imm_11_0::value;
	immediate(U::InUnpackedImmediate::Imm_31_12::high, U::InUnpackedImmediate::Imm_31_12::low) = packed_immediate(U::InPackedImmediate::Imm_31_12::high, U::InPackedImmediate::Imm_31_12::low);
	return immediate;
}

word_t unpack_J_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate.set_bit(J::InUnpackedImmediate::Imm_0::pos, J::InUnpackedImmediate::Imm_0::value);
	immediate(J::InUnpackedImmediate::Imm_20_1::high, J::InUnpackedImmediate::Imm_20_1::low) = packed_immediate(J::InPackedImmediate::Imm_20_1::high, J::InPackedImmediate::Imm_20_1::low);
	immediate(J::InUnpackedImmediate::SignExtension::high, J::InUnpackedImmediate::SignExtension::low) = packed_immediate.get_bit(J::InPackedImmediate::Sign::pos) ? -1 : 0;
	return immediate;
}

word_t unpack_immediate(Type type, const packed_immediate_t& packed_immediate) {
	switch (type) {
	case Type::R: break;
	case Type::I: return unpack_I_immediate(packed_immediate);
	case Type::S: return unpack_S_immediate(packed_immediate);
	case Type::B: return unpack_B_immediate(packed_immediate);
	case Type::U: return unpack_U_immediate(packed_immediate);
	case Type::J: return unpack_J_immediate(packed_immediate);
	case Type::unknown: break;
	}
	return 0;
}

void DecodeStage::interface(FetchToDecode from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, bool* stop) {
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
		const reg_t reg1 = from_fetch.instruction(Reg1::high, Reg1::low);
		const reg_t reg2 = from_fetch.instruction(Reg2::high, Reg2::low);

		const func3_t func3 = from_fetch.instruction(Func3::high, Func3::low);
		const func7_t func7 = from_fetch.instruction(Func7::high, Func7::low);
		to_issue->func3 = func3;

		const bool is_func7_0b0x0000x = !(func7 & 0b1011110);
		to_issue-> is_func7_0b0000000 = is_func7_0b0x0000x && !func7.test(5) && !func7.test(0);
		to_issue-> is_func7_0b0000001 = is_func7_0b0x0000x && !func7.test(5) &&  func7.test(0);
		to_issue-> is_func7_0b0100000 = is_func7_0b0x0000x &&  func7.test(5) && !func7.test(0);

		to_issue->type = Type::unknown;
		to_issue->kind = Kind::unknown;

		bool is_jump_instruction = false;

		if (opcode_prefix == 0b11) {
			switch (opcode_high) {
			case Opcode::Suffix::High::load_fence_auipc:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h00_load : to_issue->type = Type::I; to_issue->kind = Kind::load;          break;
				case Opcode::Suffix::Low::h00_001  :                                                                 break;
				case Opcode::Suffix::Low::h00_010  :                                                                 break;
				case Opcode::Suffix::Low::h00_fence: to_issue->type = Type::I; to_issue->kind = Kind::other;         break;
				case Opcode::Suffix::Low::h00_alui : to_issue->type = Type::I; to_issue->kind = Kind::alu_immediate; break;
				case Opcode::Suffix::Low::h00_auipc: to_issue->type = Type::U; to_issue->kind = Kind::other;         break;
				case Opcode::Suffix::Low::h00_110  :                                                                 break;
				case Opcode::Suffix::Low::h00_111  :                                                                 break;
				}
				break;
			case Opcode::Suffix::High::store_alu_lui:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h01_store: to_issue->type = Type::S; to_issue->kind = Kind::store; break;
				case Opcode::Suffix::Low::h01_001  :                                                         break;
				case Opcode::Suffix::Low::h01_010  :                                                         break;
				case Opcode::Suffix::Low::h01_011  :                                                         break;
				case Opcode::Suffix::Low::h01_alu  : to_issue->type = Type::R; to_issue->kind = Kind::alu;   break;
				case Opcode::Suffix::Low::h01_lui  : to_issue->type = Type::U; to_issue->kind = Kind::other; break;
				case Opcode::Suffix::Low::h01_110  :                                                         break;
				case Opcode::Suffix::Low::h01_111  :                                                         break;
				}
				break;
			case Opcode::Suffix::High::h10_unused:
				break;
			case Opcode::Suffix::High::branch_jal_r_system:
				switch (opcode_low) {
				case Opcode::Suffix::Low::h11_branch: to_issue->type = Type::B; to_issue->kind = Kind::branch;                            break;
				case Opcode::Suffix::Low::h11_jalr  : to_issue->type = Type::I; to_issue->kind = Kind::other;                             break;
				case Opcode::Suffix::Low::h11_010   :                                                                                     break;
				case Opcode::Suffix::Low::h11_jal   : to_issue->type = Type::J; to_issue->kind = Kind::other; is_jump_instruction = true; break;
				case Opcode::Suffix::Low::h11_system: to_issue->type = Type::I; to_issue->kind = Kind::other;                             break;
				case Opcode::Suffix::Low::h11_101   :                                                                                     break;
				case Opcode::Suffix::Low::h11_110   :                                                                                     break;
				case Opcode::Suffix::Low::h11_111   :                                                                                     break;
				}
				break;
			}
		}

		switch (to_issue->type) {
		case Type::R:
			to_issue->use_reg1 = true;
			to_issue->use_reg2 = true;
			to_issue->use_dest = true;
			break;
		case Type::I:
			to_issue->packed_immediate = pack_I_immediate(from_fetch.instruction);
			to_issue->use_reg1 = true;
			to_issue->use_dest = true;
			break;
		case Type::S:
			to_issue->packed_immediate = pack_S_immediate(from_fetch.instruction);
			to_issue->use_reg1 = true;
			to_issue->use_reg2 = true;
			break;
		case Type::B:
			to_issue->packed_immediate = pack_B_immediate(from_fetch.instruction);
			to_issue->use_reg1 = true;
			to_issue->use_reg2 = true;
			to_issue->use_dest = true;
			break;
		case Type::U:
			to_issue->packed_immediate = pack_U_immediate(from_fetch.instruction);
			to_issue->use_dest = true;
			break;
		case Type::J:
			to_issue->packed_immediate = pack_J_immediate(from_fetch.instruction);
			to_issue->use_dest = true;
			break;
		case Type::unknown:
			break;
		}

		bool blocking = false;
		if (to_issue->use_reg1)
			register_map.get_alias(dest, &to_issue->reg1);
		if (to_issue->use_reg2)
			register_map.get_alias(dest, &to_issue->reg2);
		if (to_issue->use_dest)
			register_map.create_alias(dest, &to_issue->dest, &blocking);
		// TODO : make something with blocking

		// TODO : transfer informations to commit for the ROB

		if (to_issue->type == Type::unknown) {
			*stop = true;
		}

		if (is_jump_instruction) {
			program_counter_t offset;
			offset(J::InUnpackedImmediate::Offset::high,    J::InUnpackedImmediate::Offset::low)    = to_issue->packed_immediate(J::InPackedImmediate::JumpOffset::high, J::InPackedImmediate::JumpOffset::low);
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
			reg1,
			reg2,
			unpack_immediate(to_issue->type, to_issue->packed_immediate)
		};
		Debugger::add_cycle_event({
			{ "Decode stage",
				{
					{ "Decoded instruction", instruction_to_json(instr) },
					{ "Next program counter", to_fetch->next_program_counter.to_uint() },
					{ "Is jump instruction ?", is_jump_instruction },
					{ "Is unknown instruction ?", to_issue->type == Type::unknown }
				}
			}
		});
		#endif // __SYNTHESIS__
	}
	tick();
}