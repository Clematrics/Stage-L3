#include "../include/decode.hpp"

#include <iostream>

#include "../include/instructions.hpp"
#include "../include/debugger.hpp"
#include "../include/disasm.hpp"
#include "../include/nlohmann/json.hpp"

Decode::Decode()
	: active(false)
{
}

void Decode::decode(const word_t instruction, word_t program_counter, word_t* out_program_counter, bit_t* stop_signal, DecodedInstruction* decoded) {
	#pragma HLS INLINE

	if (active) {
		#ifdef ITERATION4
		const opcode_prefix_t opcode_prefix = instruction(1, 0);
		const opcode_low_t    opcode_low    = instruction(4, 2);
		const opcode_high_t   opcode_high   = instruction(6, 5);

		const reg_t dest = instruction(11,  7);
		const reg_t reg1 = instruction(19, 15);
		const reg_t reg2 = instruction(24, 20);

		decoded->dest = dest;
		decoded->reg1 = reg1;
		decoded->reg2 = reg2;

		const func3_t func3 = instruction(14, 12);
		const func7_t func7 = instruction(31, 25);
		const bool is_func7_0b0x0000x = !(func7 & 0b1011110);
		const bool is_func7_0b0000000 = is_func7_0b0x0000x && !func7.test(5) && !func7.test(0);
		const bool is_func7_0b0000001 = is_func7_0b0x0000x && !func7.test(5) &&  func7.test(0);
		const bool is_func7_0b0100000 = is_func7_0b0x0000x &&  func7.test(5) && !func7.test(0);

		bool is_jump_instruction = false;
		bool is_ret_instruction  = false;

		if (opcode_prefix == 0b11) {
			switch (opcode_high) {
			case OPCODE_HIGH_00_LOAD_FENCE_ALUI_AUIPC:
				switch (opcode_low) {
				case OPCODE_LOW_00_LOAD :
					// I type instructions LB LH LW LBU LHU
					decoded->immediate(11,  0) = instruction(31, 20);
					decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
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
					decoded->immediate(11,  0) = instruction(31, 20);
					decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
					// For FENCE instruction, fm is on immediate(11, 8), pred is on immediate(7, 4), succ is on immediate(3, 0)

					if (func3 == 0b000) decoded->name = Instruction::FENCE;
					else                decoded->name = Instruction::UnknownName;
					break;
				case OPCODE_LOW_00_ALUI :
					// I type instructions ADDI SLLI SLTI SLTIU XORI SRLI SRAI ORI ANDI
					decoded->immediate(11,  0) = instruction(31, 20);
					decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
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
					decoded->immediate(11,  0) = 0x000;
					decoded->immediate(31, 12) = instruction(31, 12);
					decoded->immediate(11,  0) = 0x000;
					decoded->immediate(31, 12) = instruction(31, 12);
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
					decoded->immediate(4, 0)   = instruction(11,  7);
					decoded->immediate(11,  5) = instruction(31, 25);
					decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
					if (func3.test(2)) {
						decoded->name = Instruction::UnknownName;
					} else {
						switch (func3(2, 0)) {
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
					decoded->immediate(11,  0) = 0x000;
					decoded->immediate(31, 12) = instruction(31, 12);
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
					decoded->immediate.set_bit(0, false);
					decoded->immediate(4, 1)     = instruction(11,  8);
					decoded->immediate(10,  5)   = instruction(30, 25);
					decoded->immediate.set_bit(11, instruction.get_bit(7));
					decoded->immediate.set_bit(12, instruction.get_bit(31));
					decoded->immediate(31, 13)   = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
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
					decoded->immediate(11,  0) = instruction(31, 20);
					decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
					if (func3 == Func3::jalr) {
						decoded->name = Instruction::JALR;
						if (instruction(31, 20) == 0 && dest == Register::x0 && reg1 == Register::x1)
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
					decoded->immediate.set_bit(0, false);
					decoded->immediate(10,  1)   = instruction(30, 21);
					decoded->immediate.set_bit(11, instruction.get_bit(20));
					decoded->immediate(19, 12)   = instruction(19, 12);
					decoded->immediate.set_bit(20, instruction.get_bit(31));
					decoded->immediate(31, 21)   = instruction.get_bit(20) ? 0xFFFFF : 0x00000;
					decoded->name = Instruction::JAL;
					is_jump_instruction = true;
					break;
				case OPCODE_LOW_11_SYSTEM: {
					// I type instructions EBREAK ECALL
					decoded->immediate(11,  0) = instruction(31, 20);
					decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
					const ap_uint<25> no_opcode = instruction(31,7);
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

		if (is_jump_instruction) {
			*out_program_counter = program_counter + decoded->immediate;
		}
		else {
			*out_program_counter = program_counter + word_size;
		}
		*stop_signal = is_ret_instruction;

		#ifndef __SYNTHESIS__
		std::cout
			<< "Name : " << Instruction::getName(decoded->name)
			<<  ", destination : " << Register::getRegister(decoded->dest)
			<<  ", source 1 : " << Register::getRegister(decoded->reg1)
			<<  ", source 2 : " << Register::getRegister(decoded->reg2) << " "
			<< (is_jump_instruction ? "(is_jump_instruction) " : "")
			<< (is_ret_instruction ? "(is_ret_instruction) " : "")
			<< '\n';
		#endif // __SYNTHESIS__

		#endif // ITERATION4

		#ifdef ITERATION5
		const opcode_prefix_t opcode_prefix = instruction(1, 0);
		const opcode_low_t    opcode_low    = instruction(4, 2);
		const opcode_high_t   opcode_high   = instruction(6, 5);

		const reg_t dest = instruction(11,  7);
		const reg_t reg1 = instruction(19, 15);
		const reg_t reg2 = instruction(24, 20);

		decoded->dest = dest;
		decoded->reg1 = reg1;
		decoded->reg2 = reg2;

		const func3_t func3 = instruction(14, 12);
		const func7_t func7 = instruction(31, 25);
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

					if (func3 == 0b000) decoded->name = Instruction::FENCE;
					else                decoded->name = Instruction::UnknownName;
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
					decoded->immediate(11,  0) = 0x000;
					decoded->immediate(31, 12) = instruction(31, 12);
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
						switch (func3(2, 0)) {
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
						if (instruction(31, 20) == 0 && dest == Register::x0 && reg1 == Register::x1)
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
					const ap_uint<25> no_opcode = instruction(31,7);
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

		switch (type) {
		case Instruction::R:
			break;
		case Instruction::I:
			decoded->immediate(11,  0) = instruction(31, 20);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? -1 : 0;
			break;
		case Instruction::S:
			decoded->immediate(4, 0)   = instruction(11,  7);
			decoded->immediate(11,  5) = instruction(31, 25);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? -1 : 0;
			break;
		case Instruction::B:
			decoded->immediate.set_bit(0, false);
			decoded->immediate(4, 1)     = instruction(11,  8);
			decoded->immediate(10,  5)   = instruction(30, 25);
			decoded->immediate.set_bit(11, instruction.get_bit(7));
			decoded->immediate.set_bit(12, instruction.get_bit(31));
			decoded->immediate(31, 13)   = instruction.get_bit(31) ? -1 : 0;
			break;
		case Instruction::U:
			decoded->immediate(11,  0) = 0x000;
			decoded->immediate(31, 12) = instruction(31, 12);
			break;
		case Instruction::J:
			decoded->immediate.set_bit(0, false);
			decoded->immediate(10,  1)   = instruction(30, 21);
			decoded->immediate.set_bit(11, instruction.get_bit(20));
			decoded->immediate(19, 12)   = instruction(19, 12);
			decoded->immediate.set_bit(20, instruction.get_bit(31));
			decoded->immediate(31, 21)   = instruction.get_bit(31) ? -1 : 0;
			break;
		case Instruction::UnknownType:
			break;
		}

		if (is_jump_instruction) {
			word_t offset;
			offset(29, 0) = decoded->immediate(31, 2);
			offset.set_bit(30, decoded->immediate.get_bit(31));
			offset.set_bit(31, decoded->immediate.get_bit(31));
			*out_program_counter = program_counter + offset;
		}
		else {
			*out_program_counter = program_counter + 1;
		}
		*stop_signal = is_ret_instruction;

		#ifndef __SYNTHESIS__
		opcode_suffix_t opcode_suffix; opcode_suffix(4, 3) = opcode_high; opcode_suffix(2, 0) = opcode_low;
		opcode_t opcode; opcode(6, 2) = opcode_suffix; opcode(1, 0) = opcode_prefix;
		AsmInstruction instr = {
			instruction,
			opcode,
			opcode_prefix,
			opcode_suffix,
			func3,
			func7,
			decoded->name,
			type,
			decoded->dest,
			decoded->reg1,
			decoded->reg2,
			decoded->immediate
		};
		Debugger::add_event({
			{"Decode stage",
				{
					{"Decoded instruction", instruction_to_json(instr)},
					{"Next program counter", out_program_counter->to_uint()},
					{"Is jump instruction ?", is_jump_instruction},
					{"Is return instruction ?", is_ret_instruction}
				}
			}
		});
		#endif // __SYNTHESIS__

		#endif // ITERATION5
	}
	active = !active;
}