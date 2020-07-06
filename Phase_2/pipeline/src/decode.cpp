#include "../include/decode.hpp"

#include <iostream>

#include "../include/instructions.hpp"
#include "../include/register_file.hpp"

#include "../include/debugger.hpp"
#include "../include/disasm.hpp"
#include "../include/json.hpp"

Decode::Decode(RegisterFile& register_file)
	: active(false),
	  register_file(register_file)
{
}

word_t decode_I_immediate(const word_t& instruction) {
	word_t immediate;
	immediate(11,  0) = instruction(Slicing::I::imm_11_0_high, Slicing::I::imm_11_0_low);
	immediate(31, 12) = instruction.get_bit(Slicing::I::imm_sign) ? -1 : 0;
	return immediate;
}

word_t decode_S_immediate(const word_t& instruction) {
	word_t immediate;
	immediate( 4,  0) = instruction(Slicing::S::imm_4_0_high , Slicing::S::imm_4_0_low);
	immediate(11,  5) = instruction(Slicing::S::imm_11_5_high, Slicing::S::imm_11_5_low);
	immediate(31, 12) = instruction.get_bit(Slicing::S::imm_sign) ? -1 : 0;
	return immediate;
}

word_t decode_B_immediate(const word_t& instruction) {
	word_t immediate;
	immediate.set_bit(0, false);
	immediate( 4,  1)   = instruction(Slicing::B::imm_4_1_high , Slicing::B::imm_4_1_low);
	immediate(10,  5)   = instruction(Slicing::B::imm_10_5_high, Slicing::B::imm_10_5_low);
	immediate.set_bit(11, instruction.get_bit(Slicing::B::imm_11));
	immediate.set_bit(12, instruction.get_bit(Slicing::B::imm_12));
	immediate(31, 13)   = instruction.get_bit(Slicing::B::imm_sign) ? -1 : 0;
	return immediate;
}

word_t decode_U_immediate(const word_t& instruction) {
	word_t immediate;
	immediate(11,  0) = 0x000;
	immediate(31, 12) = instruction(Slicing::U::imm_31_12_high, Slicing::U::imm_31_12_low);
	return immediate;
}

word_t decode_J_immediate(const word_t& instruction) {
	word_t immediate;
	immediate.set_bit(0, false);
	immediate(10,  1)   = instruction(Slicing::J::imm_10_1_high, Slicing::J::imm_10_1_low);
	immediate.set_bit(11, instruction.get_bit(Slicing::J::imm_11));
	immediate(19, 12)   = instruction(Slicing::J::imm_19_12_high, Slicing::J::imm_19_12_low);
	immediate.set_bit(20, instruction.get_bit(Slicing::J::imm_20));
	immediate(31, 21)   = instruction.get_bit(Slicing::J::imm_sign) ? -1 : 0;
	return immediate;
}


packed_immediate_t pack_I_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(11,  0) = instruction(Slicing::I::imm_11_0_high, Slicing::I::imm_11_0_low);
	return immediate;
}

packed_immediate_t pack_S_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate( 4,  0) = instruction(Slicing::S::imm_4_0_high , Slicing::S::imm_4_0_low);
	immediate(11,  5) = instruction(Slicing::S::imm_11_5_high, Slicing::S::imm_11_5_low);
	return immediate;
}

packed_immediate_t pack_B_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate( 3,  0)   = instruction(Slicing::B::imm_4_1_high , Slicing::B::imm_4_1_low);
	immediate( 9,  4)   = instruction(Slicing::B::imm_10_5_high, Slicing::B::imm_10_5_low);
	immediate.set_bit(10, instruction.get_bit(Slicing::B::imm_11));
	immediate.set_bit(11, instruction.get_bit(Slicing::B::imm_12));
	return immediate;
}

packed_immediate_t pack_U_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(19,  0)   = instruction(Slicing::U::imm_31_12_high, Slicing::U::imm_31_12_low);
	return immediate;
}

packed_immediate_t pack_J_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate( 9,  0)   = instruction(Slicing::J::imm_10_1_high, Slicing::J::imm_10_1_low);
	immediate.set_bit(10, instruction.get_bit(Slicing::J::imm_11));
	immediate(18, 11)   = instruction(Slicing::J::imm_19_12_high, Slicing::J::imm_19_12_low);
	immediate.set_bit(19, instruction.get_bit(Slicing::J::imm_20));
	return immediate;
}

word_t unpack_I_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(11,  0) = packed_immediate(11, 0);
	immediate(31, 12) = packed_immediate.test(11) ? -1 : 0;
	return immediate;
}

word_t unpack_S_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(11,  0) = packed_immediate(11, 0);
	immediate(31, 12) = packed_immediate.test(11) ? -1 : 0;
	return immediate;
}

word_t unpack_B_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate.set_bit(0, false);
	immediate(12,  1)   = packed_immediate(11, 0);
	immediate(31, 13)   = packed_immediate.test(11) ? -1 : 0;
	return immediate;
}

word_t unpack_U_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(11,  0) = 0x000;
	immediate(31, 12) = packed_immediate;
	return immediate;
}

word_t unpack_J_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate.set_bit(0, false);
	immediate(20,  1) = packed_immediate;
	immediate(31, 21) = packed_immediate.get_bit(19) ? -1 : 0;
	return immediate;
}


word_t unpack_immediate(Instruction::Type type, const packed_immediate_t& packed_immediate) {
	switch (type) {
	case Instruction::R: break;
	case Instruction::I: return unpack_I_immediate(packed_immediate);
	case Instruction::S: return unpack_S_immediate(packed_immediate);
	case Instruction::B: return unpack_B_immediate(packed_immediate);
	case Instruction::U: return unpack_U_immediate(packed_immediate);
	case Instruction::J: return unpack_J_immediate(packed_immediate);
	case Instruction::UnknownType: break;
	}
	return 0;
}

void Decode::decode(const word_t& instruction, const word_t& program_counter, word_t* out_program_counter, bit_t* stop_signal, DecodedInstruction* decoded) {
	#pragma HLS INLINE

	if (active) {
		const opcode_prefix_t      opcode_prefix = instruction(Slicing::OpcodePrefix::high    , Slicing::OpcodePrefix::low);
		const opcode_suffix_low_t  opcode_low    = instruction(Slicing::OpcodeSuffixLow::high , Slicing::OpcodeSuffixLow::low);
		const opcode_suffix_high_t opcode_high   = instruction(Slicing::OpcodeSuffixHigh::high, Slicing::OpcodeSuffixHigh::low);

		const reg_t dest = instruction(Slicing::Dest::high, Slicing::Dest::low);
		const reg_t reg1 = instruction(Slicing::Reg1::high, Slicing::Reg1::low);
		const reg_t reg2 = instruction(Slicing::Reg2::high, Slicing::Reg2::low);

		const func3_t func3 = instruction(Slicing::Func3::high, Slicing::Func3::low);
		const func7_t func7 = instruction(Slicing::Func7::high, Slicing::Func7::low);

		const bool is_func7_0b0x0000x = !(func7 & 0b1011110);
		const bool is_func7_0b0000000 = is_func7_0b0x0000x && !func7.test(5) && !func7.test(0);
		const bool is_func7_0b0000001 = is_func7_0b0x0000x && !func7.test(5) &&  func7.test(0);
		const bool is_func7_0b0100000 = is_func7_0b0x0000x &&  func7.test(5) && !func7.test(0);

		Instruction::Type type = Instruction::UnknownType;

		bool is_jump_instruction = false;
		bool is_ret_instruction  = false;

		if (opcode_prefix == 0b11) {
			switch (opcode_high) {
			case OPCODE_HIGH_00_LOAD_FENCE_ALUI_AUIPC:
				switch (opcode_low) {
				case OPCODE_LOW_00_LOAD :
					// I type instructions LB LH LW LBU LHU
					type = Instruction::I;
					switch (func3) {
					case FUNC3_LB:   decoded->name = Instruction::LB;  break;
					case FUNC3_LH:   decoded->name = Instruction::LH;  break;
					case FUNC3_LW:   decoded->name = Instruction::LW;  break;
					case FUNC3_L011: decoded->name = Instruction::UnknownName; break;
					case FUNC3_LBU:  decoded->name = Instruction::LBU; break;
					case FUNC3_LHU:  decoded->name = Instruction::LHU; break;
					case FUNC3_L110: decoded->name = Instruction::UnknownName; break;
					case FUNC3_L111: decoded->name = Instruction::UnknownName; break;
					}
					break;
				case OPCODE_LOW_00_001  :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_00_010  :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_00_FENCE:
					// I type instruction FENCE
					type = Instruction::I;
					// For FENCE instruction, fm is on immediate(11, 8), pred is on immediate(7, 4), succ is on immediate(3, 0)

					if (func3 == Func3::fence) decoded->name = Instruction::FENCE;
					else                       decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_00_ALUI :
					// I type instructions ADDI SLLI SLTI SLTIU XORI SRLI SRAI ORI ANDI
					type = Instruction::I;
					// The shift amount for SLLI, SRLI and SRAI instructions is now immediate(4, 0)

					switch (func3) {
					case FUNC3_ADDI:                 decoded->name = Instruction::ADDI;  break;
					case FUNC3_SLLI:
						if      (is_func7_0b0000000) decoded->name = Instruction::SLLI;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_SLTI:                 decoded->name = Instruction::SLTI;  break;
					case FUNC3_SLTIU:                decoded->name = Instruction::SLTIU; break;
					case FUNC3_XORI:                 decoded->name = Instruction::XORI;  break;
					case FUNC3_SRLI_SRAI:
						if      (is_func7_0b0000000) decoded->name = Instruction::SRLI;
						else if (is_func7_0b0100000) decoded->name = Instruction::SRAI;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_ORI:                  decoded->name = Instruction::ORI;   break;
					case FUNC3_ANDI:                 decoded->name = Instruction::ANDI;  break;
					}
					break;
				case OPCODE_LOW_00_AUIPC:
					// U type instructions AUIPC
					type = Instruction::U;
					decoded->name = Instruction::AUIPC;
					break;
				case OPCODE_LOW_00_110  :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_00_111  :
					decoded->name = Instruction::UnknownName;
					break;
				}
				break;
			case OPCODE_HIGH_01_STORE_ALU_LUI:
				switch (opcode_low) {
				case OPCODE_LOW_01_STORE:
					// S type instructions SB SH SW
					type = Instruction::S;
					if (func3.test(2)) {
						decoded->name = Instruction::UnknownName;
					} else {
						switch (func3(1, 0)) {
						case 0b00: decoded->name = Instruction::SB;  break;
						case 0b01: decoded->name = Instruction::SH;  break;
						case 0b10: decoded->name = Instruction::SW;  break;
						case 0b11: decoded->name = Instruction::UnknownName; break;
						}
					}
					break;
				case OPCODE_LOW_01_001  :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_01_010  :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_01_011  :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_01_ALU  :
					// R type instructions ADD SUB SLL SLT SLTU XOR SRL SRA OR AND MUL MULH MULHSU MULHU DIV DIVU REM REMU
					type = Instruction::R;
					switch (func3) {
					case FUNC3_ADD_SUB_MUL:
						if      (is_func7_0b0000000) decoded->name = Instruction::ADD;
						else if (is_func7_0b0100000) decoded->name = Instruction::SUB;
						else if (is_func7_0b0000001) decoded->name = Instruction::MUL;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_SLL_MULH:
						if      (is_func7_0b0000000) decoded->name = Instruction::SLL;
						else if (is_func7_0b0000001) decoded->name = Instruction::MULH;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_SLT_MULHSU:
						if      (is_func7_0b0000000) decoded->name = Instruction::SLT;
						else if (is_func7_0b0000001) decoded->name = Instruction::MULHSU;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_SLTU_MULHU:
						if      (is_func7_0b0000000) decoded->name = Instruction::SLTU;
						else if (is_func7_0b0000001) decoded->name = Instruction::MULHU;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_XOR_DIV:
						if      (is_func7_0b0000000) decoded->name = Instruction::XOR;
						else if (is_func7_0b0000001) decoded->name = Instruction::DIV;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_SRL_SRA_DIVU:
						if      (is_func7_0b0000000) decoded->name = Instruction::SRL;
						else if (is_func7_0b0100000) decoded->name = Instruction::SRA;
						else if (is_func7_0b0000001) decoded->name = Instruction::DIVU;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_OR_REM:
						if      (is_func7_0b0000000) decoded->name = Instruction::OR;
						else if (is_func7_0b0000001) decoded->name = Instruction::REM;
						else                         decoded->name = Instruction::UnknownName;
						break;
					case FUNC3_AND_REMU:
						if      (is_func7_0b0000000) decoded->name = Instruction::AND;
						else if (is_func7_0b0000001) decoded->name = Instruction::REMU;
						else                         decoded->name = Instruction::UnknownName;
						break;
					}
					break;
				case OPCODE_LOW_01_LUI  :
					// U type instructions LUI
					type = Instruction::U;
					decoded->name = Instruction::LUI;
					break;
				case OPCODE_LOW_01_110  :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_01_111  :
					decoded->name = Instruction::UnknownName;
					break;
				}
				break;
			case OPCODE_HIGH_10:
				decoded->name = Instruction::UnknownName;
				break;
			case OPCODE_HIGH_11_BRANCH_JAL_R_SYSTEM:
				switch (opcode_low) {
				case OPCODE_LOW_11_BRANCH:
					// B type instructions BEQ BNE BLT BGE BLTU BGEU
					type = Instruction::B;
					switch (func3) {
					case FUNC3_BEQ:  decoded->name = Instruction::BEQ;  break;
					case FUNC3_BNE:  decoded->name = Instruction::BNE;  break;
					case FUNC3_B010: decoded->name = Instruction::UnknownName; break;
					case FUNC3_B011: decoded->name = Instruction::UnknownName; break;
					case FUNC3_BLT:  decoded->name = Instruction::BLT;  break;
					case FUNC3_BGE:  decoded->name = Instruction::BGE;  break;
					case FUNC3_BLTU: decoded->name = Instruction::BLTU; break;
					case FUNC3_BGEU: decoded->name = Instruction::BGEU; break;
					}
					break;
				case OPCODE_LOW_11_JALR  :
					// I type instructions JALR
					type = Instruction::I;
					if (func3 == Func3::jalr) {
						decoded->name = Instruction::JALR;
						if (instruction(Slicing::I::imm_11_0_high, Slicing::I::imm_11_0_low) == 0 && dest == Register::x0 && reg1 == Register::x1)
							is_ret_instruction = true;
					}
					else {
						decoded->name = Instruction::UnknownName;
					}
					break;
				case OPCODE_LOW_11_010   :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_11_JAL   :
					// J type instruction JAL
					type = Instruction::J;
					decoded->name = Instruction::JAL;
					is_jump_instruction = true;
					break;
				case OPCODE_LOW_11_SYSTEM: {
					// I type instructions EBREAK ECALL
					type = Instruction::I;
					const ap_uint<25> no_opcode = instruction(31, 7);
					if      (no_opcode == 0b0000000000000000000000000) decoded->name = Instruction::EBREAK;
					else if (no_opcode == 0b0000000000010000000000000) decoded->name = Instruction::ECALL;
					else                                               decoded->name = Instruction::UnknownName;
					break;
					}
				case OPCODE_LOW_11_101   :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_11_110   :
					decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_11_111   :
					decoded->name = Instruction::UnknownName;
					break;
				}
				break;
			}
		} else {
			decoded->name = Instruction::UnknownName;
		}

		bool rename_reg1 = false;
		bool rename_reg2 = false;
		bool rename_dest = false;
		switch (type) {
		case Instruction::R:
			rename_reg1 = true;
			rename_reg2 = true;
			rename_dest = true;
			break;
		case Instruction::I:
			decoded->packed_immediate = pack_I_immediate(instruction);
			rename_reg1 = true;
			rename_dest = true;
			break;
		case Instruction::S:
			decoded->packed_immediate = pack_S_immediate(instruction);
			rename_reg1 = true;
			rename_reg2 = true;
			break;
		case Instruction::B:
			decoded->packed_immediate = pack_B_immediate(instruction);
			rename_reg1 = true;
			rename_reg2 = true;
			rename_dest = true;
			break;
		case Instruction::U:
			decoded->packed_immediate = pack_U_immediate(instruction);
			rename_dest = true;
			break;
		case Instruction::J:
			decoded->packed_immediate = pack_J_immediate(instruction);
			rename_dest = true;
			break;
		case Instruction::UnknownType:
			break;
		}

		bool blocking = false;
		if (rename_reg1)
			register_file.get_alias(dest, &decoded->reg1);
		if (rename_reg2)
			register_file.get_alias(dest, &decoded->reg2);
		if (rename_dest)
			register_file.create_alias(dest, &decoded->dest, &blocking);

		if (is_jump_instruction) {
			word_t offset;
			offset(18,  0) = decoded->packed_immediate(19, 1);
			offset(31, 19) = decoded->packed_immediate.test(19) ? -1 : 0;
			*out_program_counter = program_counter + offset;
		}
		else {
			*out_program_counter = program_counter + 1;
		}
		*stop_signal = is_ret_instruction;

		#ifndef __SYNTHESIS__
		opcode_suffix_t opcode_suffix = instruction(Slicing::OpcodeSuffix::high, Slicing::OpcodeSuffix::low);
		opcode_t opcode               = instruction(Slicing::Opcode::high, Slicing::Opcode::low);
		AsmInstruction instr = {
			instruction,
			opcode,
			opcode_prefix,
			opcode_suffix,
			func3,
			func7,
			decoded->name,
			type,
			dest,
			reg1,
			reg2,
			unpack_immediate(type, decoded->packed_immediate)
		};
		Debugger::add_cycle_event({
			{ "Decode stage",
				{
					{ "Decoded instruction", instruction_to_json(instr) },
					{ "Next program counter", out_program_counter->to_uint() },
					{ "Is jump instruction ?", is_jump_instruction },
					{ "Is return instruction ?", is_ret_instruction }
				}
			}
		});
		#endif // __SYNTHESIS__
	}
	active = !active;
}