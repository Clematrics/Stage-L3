#include "../include/decode.hpp"
#include "../include/instructions.hpp"

#include <iostream>

Decode::Decode()
	: active(false)
{
}

void Decode::decode(const word_t instruction, word_t program_counter, word_t* out_program_counter, bit_t* stop_signal, DecodedInstruction* decoded) {
	#pragma HLS INLINE

	if (active) {
		#ifdef ITERATION1
		ap_uint<7> opcode = instruction(6, 0);
		decoded->opcode     = opcode;

		if (opcode == 0b0010011) {
			ap_uint<3> func3 = instruction(14, 12);
			if (func3 == 0b000 || func3 == 0b010 || func3 == 0b011 || func3 == 0b100 || func3 == 0b110 || func3 == 0b111) {
				// I type instructions ADDI SLTI SLTIU XORI ORI ANDI
				decoded->dest  = instruction(11,  7);
				decoded->func3 = instruction(14, 12);
				decoded->reg1  = instruction(19, 15);
				decoded->immediate(11,  0) = instruction(31, 20);
				decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			}
			else {
				// R type instructions SLLI SRLI SRAI
				decoded->dest  = instruction(11,  7);
				decoded->func3 = instruction(14, 12);
				decoded->reg1  = instruction(19, 15);
				decoded->reg2  = instruction(24, 20);
				decoded->func7 = instruction(31, 25);
			}
		}
		else if (opcode == 0b0110011) {
			// R type instructions ADD SUB SLL SLT SLTU XOR SRL SRA OR AND MUL MULH MULHSU MULHU DIV DIVU REM REMU
			decoded->dest  = instruction(11,  7);
			decoded->func3 = instruction(14, 12);
			decoded->reg1  = instruction(19, 15);
			decoded->reg2  = instruction(24, 20);
			decoded->func7 = instruction(31, 25);
		}
		else if (opcode == 0b1100111 || opcode == 0b0000011 || opcode == 0b0001111 || opcode == 0b1110011) {
			// I type instructions JALR LB LH LW LBU LHU FENCE ECALL EBREAK
			if      (instruction == 0b00000000000000000000000001110011) {
				// ECALL
			}
			else if (instruction == 0b00000000000100000000000001110011) {
				// ECALL
			}
			else {
				decoded->dest  = instruction(11,  7);
				decoded->func3 = instruction(14, 12);
				decoded->reg1  = instruction(19, 15);
				decoded->immediate(11,  0) = instruction(31, 20);
				decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			}
		}
		else if (opcode == 0b0100011) {
			// S type instructions SB SH SW
			decoded->immediate(4, 0)   = instruction(11,  7);
			decoded->func3             = instruction(14, 12);
			decoded->reg1              = instruction(19, 15);
			decoded->reg2              = instruction(24, 20);
			decoded->immediate(11,  5) = instruction(31, 25);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
		}
		else if (opcode == 0b1100011) {
			// B type instructions BEQ BNE BLT BGE BLTU BGEU
			decoded->immediate(4, 1)   = instruction(11,  8);
			decoded->func3             = instruction(14, 12);
			decoded->reg1              = instruction(19, 15);
			decoded->reg2              = instruction(24, 20);
			decoded->immediate(10,  5) = instruction(30, 25);
			decoded->immediate(31, 13) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			decoded->immediate.set_bit(11, instruction.get_bit(7));
			decoded->immediate.set_bit(12, instruction.get_bit(31));
			decoded->immediate.set_bit(0, false);
		}
		else if (opcode == 0b0110111 || opcode == 0b0010111) {
			// U type instructions LUI AUIPC
			decoded->dest  = instruction(11,  7);
			decoded->immediate(31, 12) = instruction(31, 12);
		}
		else if (opcode == 0b1101111) {
			// J type instruction JAL
			decoded->dest  = instruction(11,  7);
			decoded->immediate.set_bit(0, false);
			decoded->immediate(19, 12) = instruction(19, 12);
			decoded->immediate(10,  1) = instruction(30, 21);
			decoded->immediate.set_bit(11, instruction.get_bit(20));
			decoded->immediate.set_bit(20, instruction.get_bit(31));
			decoded->immediate(31, 21) = instruction.get_bit(20) ? 0xFFFFF : 0x00000;
		}
		else {
			decoded->invalid = true;
		}

		*out_program_counter = program_counter + word_size;
		*stop_signal = instruction == 0xffffffff ? true : false;
		#endif // ITERATION1

		#ifdef ITERATION2
		ap_uint<7> opcode   = instruction(6, 0);
		decoded->dest  = instruction(11,  7);
		decoded->reg1  = instruction(19, 15);
		decoded->reg2  = instruction(24, 20);

		ap_uint<3> func3 = instruction(14, 12);
		ap_uint<7> func7 = instruction(31, 25);

		if (opcode == 0b0010011) {
			decoded->immediate(11,  0) = instruction(31, 20);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			// The shift amount for SLLI, SRLI and SRAI instructions is now immediate(4, 0)

			switch (func3) {
			case 0b000:                      decoded->name = Instruction::ADDI;  break;
			case 0b001:
				if      (func7 == 0b0000000) decoded->name = Instruction::SLLI;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b010:                      decoded->name = Instruction::SLTI;  break;
			case 0b011:                      decoded->name = Instruction::SLTIU; break;
			case 0b100:                      decoded->name = Instruction::XORI;  break;
			case 0b101:
				if      (func7 == 0b0000000) decoded->name = Instruction::SRLI;
				else if (func7 == 0b0100000) decoded->name = Instruction::SRAI;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b110:                      decoded->name = Instruction::ORI;   break;
			case 0b111:                      decoded->name = Instruction::ANDI;  break;
			default: break; // Cannot happen
			}
		}
		else if (opcode == 0b0110011) {
			// R type instructions ADD SUB SLL SLT SLTU XOR SRL SRA OR AND MUL MULH MULHSU MULHU DIV DIVU REM REMU
			switch (func3) {
			case 0b000:
				if      (func7 == 0b0000000) decoded->name = Instruction::ADD;
				else if (func7 == 0b0100000) decoded->name = Instruction::SUB;
				else if (func7 == 0b0000001) decoded->name = Instruction::MUL;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b001:
				if      (func7 == 0b0000000) decoded->name = Instruction::SLL;
				else if (func7 == 0b0000001) decoded->name = Instruction::MULH;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b010:
				if      (func7 == 0b0000000) decoded->name = Instruction::SLT;
				else if (func7 == 0b0000001) decoded->name = Instruction::MULHSU;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b011:
				if      (func7 == 0b0000000) decoded->name = Instruction::SLTU;
				else if (func7 == 0b0000001) decoded->name = Instruction::MULHU;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b100:
				if      (func7 == 0b0000000) decoded->name = Instruction::XOR;
				else if (func7 == 0b0000001) decoded->name = Instruction::DIV;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b101:
				if      (func7 == 0b0000000) decoded->name = Instruction::SRL;
				else if (func7 == 0b0100000) decoded->name = Instruction::SRA;
				else if (func7 == 0b0000001) decoded->name = Instruction::DIVU;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b110:
				if      (func7 == 0b0000000) decoded->name = Instruction::OR;
				else if (func7 == 0b0000001) decoded->name = Instruction::REM;
				else                         decoded->name = Instruction::Unknown;
				break;
			case 0b111:
				if      (func7 == 0b0000000) decoded->name = Instruction::AND;
				else if (func7 == 0b0000001) decoded->name = Instruction::REMU;
				else                         decoded->name = Instruction::Unknown;
				break;
			default: break; // Cannot happen
			}
		}
		else if (opcode == 0b1100111) {
			// I type instructions JALR
			decoded->immediate(11,  0) = instruction(31, 20);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			if (func3 == 0b000) decoded->name = Instruction::JALR;
			else                decoded->name = Instruction::Unknown;
		}
		else if (opcode == 0b0000011) {
			// I type instructions LB LH LW LBU LHU
			decoded->immediate(11,  0) = instruction(31, 20);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			switch (func3) {
			case 0b000: decoded->name = Instruction::LB;  break;
			case 0b001: decoded->name = Instruction::LH;  break;
			case 0b010: decoded->name = Instruction::LW;  break;
			case 0b100: decoded->name = Instruction::LBU; break;
			case 0b101: decoded->name = Instruction::LHU; break;
			default: decoded->name = Instruction::Unknown; break;
			}
		}
		else if (opcode == 0b0001111) {
			// I type instruction FENCE
			decoded->immediate(11,  0) = instruction(31, 20);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			// For FENCE instruction, fm is on immediate(11, 8), pred is on immediate(7, 4), succ is on immediate(3, 0)

			if (func3 == 0b000) decoded->name = Instruction::FENCE;
			else                decoded->name = Instruction::Unknown;
		}
		else if (opcode == 0b1110011) {
			// I type instructions EBREAK ECALL
			if      (instruction == 0b00000000000000000000000001110011) decoded->name = Instruction::EBREAK;
			else if (instruction == 0b00000000000100000000000001110011) decoded->name = Instruction::ECALL;
			else                                                        decoded->name = Instruction::Unknown;
		}
		else if (opcode == 0b0100011) {
			// S type instructions SB SH SW
			decoded->immediate(4, 0)   = instruction(11,  7);
			decoded->immediate(11,  5) = instruction(31, 25);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			switch (func3) {
			case 0b000: decoded->name = Instruction::SB;  break;
			case 0b001: decoded->name = Instruction::SH;  break;
			case 0b010: decoded->name = Instruction::SW;  break;
			default: decoded->name = Instruction::Unknown; break;
			}
		}
		else if (opcode == 0b1100011) {
			// B type instructions BEQ BNE BLT BGE BLTU BGEU
			decoded->immediate.set_bit(0, false);
			decoded->immediate(4, 1)     = instruction(11,  8);
			decoded->immediate(10,  5)   = instruction(30, 25);
			decoded->immediate.set_bit(11, instruction.get_bit(7));
			decoded->immediate.set_bit(12, instruction.get_bit(31));
			decoded->immediate(31, 13)   = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			switch (func3) {
			case 0b000: decoded->name = Instruction::BEQ;  break;
			case 0b001: decoded->name = Instruction::BNE;  break;
			case 0b100: decoded->name = Instruction::BLT;  break;
			case 0b101: decoded->name = Instruction::BGE;  break;
			case 0b110: decoded->name = Instruction::BLTU; break;
			case 0b111: decoded->name = Instruction::BGEU; break;
			default: decoded->name = Instruction::Unknown; break;
			}
		}
		else if (opcode == 0b0110111) {
			// U type instructions LUI
			decoded->immediate(11,  0) = 0x000;
			decoded->immediate(31, 12) = instruction(31, 12);
			decoded->name = Instruction::LUI;
		}
		else if (opcode == 0b0010111) {
			// U type instructions AUIPC
			decoded->immediate(11,  0) = 0x000;
			decoded->immediate(31, 12) = instruction(31, 12);
			decoded->name = Instruction::AUIPC;
		}
		else if (opcode == 0b1101111) {
			// J type instruction JAL
			decoded->immediate.set_bit(0, false);
			decoded->immediate(10,  1)   = instruction(30, 21);
			decoded->immediate.set_bit(11, instruction.get_bit(20));
			decoded->immediate(19, 12)   = instruction(19, 12);
			decoded->immediate.set_bit(20, instruction.get_bit(31));
			decoded->immediate(31, 21)   = instruction.get_bit(20) ? 0xFFFFF : 0x00000;
			decoded->name = Instruction::JAL;
		}
		else {
			decoded->name = Instruction::Unknown;
		}

		if (decoded->name == Instruction::JAL) {
			*out_program_counter = program_counter + decoded->immediate;
		}
		else {
			*out_program_counter = program_counter + word_size;
		}

		// when the ret (jalr x0, 0(x1)) instruction is met, a stop signal is sent
		*stop_signal = decoded->name == Instruction::JALR && decoded->dest == 0x0 && decoded->reg1 == 0x1 && decoded->immediate == 0x0 ? true : false;
		#endif // ITERATION2

		#ifdef ITERATION3
		const opcode_prefix_t opcode_prefix = instruction(1, 0);
		const opcode_suffix_t opcode_suffix = instruction(6, 2);

		const reg_t dest = instruction(11,  7);
		const reg_t reg1 = instruction(19, 15);
		const reg_t reg2 = instruction(24, 20);

		decoded->dest = dest;
		decoded->reg1 = reg1;
		decoded->reg2 = reg2;

		const func3_t func3 = instruction(14, 12);
		const func7_t func7 = instruction(31, 25);
		const bool is_func7_null  = func7 == Func7::null;
		const bool is_func7_first = func7 == Func7::first;
		const bool is_func7_sixth = func7 == Func7::sixth;

		bool is_jump_instruction = false;
		bool is_ret_instruction  = false;

		if (opcode_prefix == 0b11) {
			if      (opcode_suffix == Opcode32Suffix::lui) {
				// U type instructions LUI
				decoded->immediate(11,  0) = 0x000;
				decoded->immediate(31, 12) = instruction(31, 12);
				decoded->name = Instruction::LUI;
			}
			else if (opcode_suffix == Opcode32Suffix::auipc) {
				// U type instructions AUIPC
				decoded->immediate(11,  0) = 0x000;
				decoded->immediate(31, 12) = instruction(31, 12);
				decoded->name = Instruction::AUIPC;
			}
			else if (opcode_suffix == Opcode32Suffix::jal) {
				// J type instruction JAL
				decoded->immediate.set_bit(0, false);
				decoded->immediate(10,  1)   = instruction(30, 21);
				decoded->immediate.set_bit(11, instruction.get_bit(20));
				decoded->immediate(19, 12)   = instruction(19, 12);
				decoded->immediate.set_bit(20, instruction.get_bit(31));
				decoded->immediate(31, 21)   = instruction.get_bit(20) ? 0xFFFFF : 0x00000;
				decoded->name = Instruction::JAL;
				is_jump_instruction = true;
			}
			else if (opcode_suffix == Opcode32Suffix::jalr) {
				// I type instructions JALR
				decoded->immediate(11,  0) = instruction(31, 20);
				decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
				if (func3 == Func3::jalr) {
					decoded->name = Instruction::JALR;
					if (instruction(31, 20) == 0 && dest == Register::x0 && reg1 == Register::x1)
						is_ret_instruction = true;
				}
				else {
					decoded->name = Instruction::Unknown;
				}
			}
			else if (opcode_suffix == Opcode32Suffix::branch) {
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
				case FUNC3_B010: decoded->name = Instruction::Unknown; break;
				case FUNC3_B011: decoded->name = Instruction::Unknown; break;
				case FUNC3_BLT:  decoded->name = Instruction::BLT;  break;
				case FUNC3_BGE:  decoded->name = Instruction::BGE;  break;
				case FUNC3_BLTU: decoded->name = Instruction::BLTU; break;
				case FUNC3_BGEU: decoded->name = Instruction::BGEU; break;
				}
			}
			else if (opcode_suffix == Opcode32Suffix::load) {
				// I type instructions LB LH LW LBU LHU
				decoded->immediate(11,  0) = instruction(31, 20);
				decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
				switch (func3) {
				case FUNC3_LB:   decoded->name = Instruction::LB;  break;
				case FUNC3_LH:   decoded->name = Instruction::LH;  break;
				case FUNC3_LW:   decoded->name = Instruction::LW;  break;
				case FUNC3_L011: decoded->name = Instruction::Unknown; break;
				case FUNC3_LBU:  decoded->name = Instruction::LBU; break;
				case FUNC3_LHU:  decoded->name = Instruction::LHU; break;
				case FUNC3_L110: decoded->name = Instruction::Unknown; break;
				case FUNC3_L111: decoded->name = Instruction::Unknown; break;
				}
			}
			else if (opcode_suffix == Opcode32Suffix::store) {
				// S type instructions SB SH SW
				decoded->immediate(4, 0)   = instruction(11,  7);
				decoded->immediate(11,  5) = instruction(31, 25);
				decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
				if (func3.test(2)) {
					decoded->name = Instruction::Unknown;
				} else {
					switch (func3(2, 0)) {
					case 0b00: decoded->name = Instruction::SB;  break;
					case 0b01: decoded->name = Instruction::SH;  break;
					case 0b10: decoded->name = Instruction::SW;  break;
					case 0b11: decoded->name = Instruction::Unknown; break;
					}
				}
			}
			else if (opcode_suffix == Opcode32Suffix::alui) {
				decoded->immediate(11,  0) = instruction(31, 20);
				decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
				// The shift amount for SLLI, SRLI and SRAI instructions is now immediate(4, 0)

				switch (func3) {
				case FUNC3_ADDI:             decoded->name = Instruction::ADDI;  break;
				case FUNC3_SLLI:
					if      (is_func7_null)  decoded->name = Instruction::SLLI;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_SLTI:             decoded->name = Instruction::SLTI;  break;
				case FUNC3_SLTIU:            decoded->name = Instruction::SLTIU; break;
				case FUNC3_XORI:             decoded->name = Instruction::XORI;  break;
				case FUNC3_SRLI_SRAI:
					if      (is_func7_null)  decoded->name = Instruction::SRLI;
					else if (is_func7_sixth) decoded->name = Instruction::SRAI;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_ORI:               decoded->name = Instruction::ORI;   break;
				case FUNC3_ANDI:              decoded->name = Instruction::ANDI;  break;
				}
			}
			else if (opcode_suffix == Opcode32Suffix::alu) {
				// R type instructions ADD SUB SLL SLT SLTU XOR SRL SRA OR AND MUL MULH MULHSU MULHU DIV DIVU REM REMU
				switch (func3) {
				case FUNC3_ADD_SUB_MUL:
					if      (is_func7_null)  decoded->name = Instruction::ADD;
					else if (is_func7_sixth) decoded->name = Instruction::SUB;
					else if (is_func7_first) decoded->name = Instruction::MUL;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_SLL_MULH:
					if      (is_func7_null)  decoded->name = Instruction::SLL;
					else if (is_func7_first) decoded->name = Instruction::MULH;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_SLT_MULHSU:
					if      (is_func7_null)  decoded->name = Instruction::SLT;
					else if (is_func7_first) decoded->name = Instruction::MULHSU;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_SLTU_MULHU:
					if      (is_func7_null)  decoded->name = Instruction::SLTU;
					else if (is_func7_first) decoded->name = Instruction::MULHU;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_XOR_DIV:
					if      (is_func7_null)  decoded->name = Instruction::XOR;
					else if (is_func7_first) decoded->name = Instruction::DIV;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_SRL_SRA_DIVU:
					if      (is_func7_null)  decoded->name = Instruction::SRL;
					else if (is_func7_sixth) decoded->name = Instruction::SRA;
					else if (is_func7_first) decoded->name = Instruction::DIVU;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_OR_REM:
					if      (is_func7_null)  decoded->name = Instruction::OR;
					else if (is_func7_first) decoded->name = Instruction::REM;
					else                     decoded->name = Instruction::Unknown;
					break;
				case FUNC3_AND_REMU:
					if      (is_func7_null)  decoded->name = Instruction::AND;
					else if (is_func7_first) decoded->name = Instruction::REMU;
					else                      decoded->name = Instruction::Unknown;
					break;
				}
			}
			else if (opcode_suffix == Opcode32Suffix::fence) {
				// I type instruction FENCE
				decoded->immediate(11,  0) = instruction(31, 20);
				decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
				// For FENCE instruction, fm is on immediate(11, 8), pred is on immediate(7, 4), succ is on immediate(3, 0)

				if (func3 == 0b000) decoded->name = Instruction::FENCE;
				else                decoded->name = Instruction::Unknown;
			}
			else if (opcode_suffix == Opcode32Suffix::system) {
				// I type instructions EBREAK ECALL
				const ap_uint<25> no_opcode = instruction(31,7);
				if      (no_opcode == 0b0000000000000000000000000) decoded->name = Instruction::EBREAK;
				else if (no_opcode == 0b0000000000010000000000000) decoded->name = Instruction::ECALL;
				else                                               decoded->name = Instruction::Unknown;
			}
			else {
				decoded->name = Instruction::Unknown;
			}
		} else {
			decoded->name = Instruction::Unknown;
		}

		if (is_jump_instruction) {
			*out_program_counter = program_counter + decoded->immediate;
		}
		else {
			*out_program_counter = program_counter + word_size;
		}
		*stop_signal = is_ret_instruction ? true : false;

		#endif // ITERATION3

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
			decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			break;
		case Instruction::S:
			decoded->immediate(4, 0)   = instruction(11,  7);
			decoded->immediate(11,  5) = instruction(31, 25);
			decoded->immediate(31, 12) = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
			break;
		case Instruction::B:
			decoded->immediate.set_bit(0, false);
			decoded->immediate(4, 1)     = instruction(11,  8);
			decoded->immediate(10,  5)   = instruction(30, 25);
			decoded->immediate.set_bit(11, instruction.get_bit(7));
			decoded->immediate.set_bit(12, instruction.get_bit(31));
			decoded->immediate(31, 13)   = instruction.get_bit(31) ? 0xFFFFF : 0x00000;
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
			decoded->immediate(31, 21)   = instruction.get_bit(20) ? 0xFFFFF : 0x00000;
			break;
		case Instruction::UnknownType:
			break;
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
			<<  ", type : " << Instruction::getType(type)
			<<  ", destination : " << Register::getRegister(decoded->dest)
			<<  ", source 1 : " << Register::getRegister(decoded->reg1)
			<<  ", source 2 : " << Register::getRegister(decoded->reg2) << " "
			<< (is_jump_instruction ? "(is_jump_instruction) " : "")
			<< (is_ret_instruction ? "(is_ret_instruction) " : "")
			<< '\n';
		#endif // __SYNTHESIS__

		#endif // ITERATION5
	}
	active = !active;
}

std::string Instruction::getType(Type type) {
	switch (type) {
	case R: return "R";
	case I: return "I";
	case S: return "S";
	case B: return "B";
	case U: return "U";
	case J: return "J";
	case UnknownType: return "Unknown";
	}
}

std::string Instruction::getName(Name name) {
	switch (name) {
	case LUI: return "LUI";
	case AUIPC: return "AUIPC";
	case JAL: return "JAL";
	case JALR: return "JALR";
	case BEQ: return "BEQ";
	case BNE: return "BNE";
	case BLT: return "BLT";
	case BGE: return "BGE";
	case BLTU: return "BLTU";
	case BGEU: return "BGEU";
	case LB: return "LB";
	case LH: return "LH";
	case LW: return "LW";
	case LBU: return "LBU";
	case LHU: return "LHU";
	case SB: return "SB";
	case SH: return "SH";
	case SW: return "SW";
	case ADDI: return "ADDI";
	case SLTI: return "SLTI";
	case SLTIU: return "SLTIU";
	case XORI: return "XORI";
	case ORI: return "ORI";
	case ANDI: return "ANDI";
	case SLLI: return "SLLI";
	case SRLI: return "SRLI";
	case SRAI: return "SRAI";
	case ADD: return "ADD";
	case SUB: return "SUB";
	case SLL: return "SLL";
	case SLT: return "SLT";
	case SLTU: return "SLTU";
	case XOR: return "XOR";
	case SRL: return "SRL";
	case SRA: return "SRA";
	case OR: return "OR";
	case AND: return "AND";
	case FENCE: return "FENCE";
	case ECALL: return "ECALL";
	case EBREAK: return "EBREAK";
	case MUL: return "MUL";
	case MULH: return "MULH";
	case MULHSU: return "MULHSU";
	case MULHU: return "MULHU";
	case DIV: return "DIV";
	case DIVU: return "DIVU";
	case REM: return "REM";
	case REMU: return "REMU";
	case UnknownName: return "Unknown";
	}
}

std::string Register::getRegister(reg_t reg) {
	switch (reg) {
	case  0: return  "x0";
	case  1: return  "x1";
	case  2: return  "x2";
	case  3: return  "x3";
	case  4: return  "x4";
	case  5: return  "x5";
	case  6: return  "x6";
	case  7: return  "x7";
	case  8: return  "x8";
	case  9: return  "x9";
	case 10: return "x10";
	case 11: return "x11";
	case 12: return "x12";
	case 13: return "x13";
	case 14: return "x14";
	case 15: return "x15";
	case 16: return "x16";
	case 17: return "x17";
	case 18: return "x18";
	case 19: return "x19";
	case 20: return "x20";
	case 21: return "x21";
	case 22: return "x22";
	case 23: return "x23";
	case 24: return "x24";
	case 25: return "x25";
	case 26: return "x26";
	case 27: return "x27";
	case 28: return "x28";
	case 29: return "x29";
	case 30: return "x30";
	case 31: return "x31";
	default: return "Unknown";
	}
}