#include "../include/decode.hpp"

Decode::Decode()
	: active(false)
{
}

#ifdef OTHER
InstructionType Decode::instructionType(const word_t instruction) {
	ap_uint<6> opcode = instruction(6, 0);
	if (opcode == 0b0010011) {
		ap_uint<3> func3 = instruction(14, 12);
		if (func3 == 0b000 || func3 == 0b010 || func3 == 0b011 || func3 == 0b100 || func3 == 0b110 || func3 == 0b111)
			return I;
		else
			return R;
	}
	else if (opcode == 0b0110011) {
		return R;
	}
	else if (opcode == 0b1100111 || opcode == 0b0000011 || opcode == 0b0001111 || opcode == 0b1110011) {
		return I;
	}
	else if (opcode == 0b0100011) {
		return S;
	}
	else if (opcode == 0b1100011) {
		return B;
	}
	else if (opcode == 0b0110111 || opcode == 0b0010111) {
		return U;
	}
	else if (opcode == 0b1101111) {
		return J;
	}
	return Unknwon;
}
#endif

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

		#ifdef OTHER
		// InstructionType type = instructionType(instruction);
		// switch (type) {
		// case R:

		// 	break;
		// case I:
		// 	break;
		// case S:
		// 	break;
		// case B:
		// 	break;
		// case U:
		// 	break;
		// case J:
		// 	break;
		// default:
		// 	decode.invalid = true;
		// 	break;
		// }
		#endif // OTHER
	}
	active = !active;
}
