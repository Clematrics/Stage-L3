#include "../include/instructions.hpp"

std::string to_string(Instruction::Type type) {
	switch (type) {
	case Instruction::R: return "R";
	case Instruction::I: return "I";
	case Instruction::S: return "S";
	case Instruction::B: return "B";
	case Instruction::U: return "U";
	case Instruction::J: return "J";
	case Instruction::UnknownType: return "Unknown";
	}
}

std::string to_string(Instruction::Name name) {
	switch (name) {
	case Instruction::LUI:         return "lui";
	case Instruction::AUIPC:       return "auipc";
	case Instruction::JAL:         return "jal";
	case Instruction::JALR:        return "jalr";
	case Instruction::BEQ:         return "beq";
	case Instruction::BNE:         return "bne";
	case Instruction::BLT:         return "blt";
	case Instruction::BGE:         return "bge";
	case Instruction::BLTU:        return "bltu";
	case Instruction::BGEU:        return "bgeu";
	case Instruction::LB:          return "lb";
	case Instruction::LH:          return "lh";
	case Instruction::LW:          return "lw";
	case Instruction::LBU:         return "lbu";
	case Instruction::LHU:         return "lhu";
	case Instruction::SB:          return "sb";
	case Instruction::SH:          return "sh";
	case Instruction::SW:          return "sw";
	case Instruction::ADDI:        return "addi";
	case Instruction::SLTI:        return "slti";
	case Instruction::SLTIU:       return "sltiu";
	case Instruction::XORI:        return "xori";
	case Instruction::ORI:         return "ori";
	case Instruction::ANDI:        return "andi";
	case Instruction::SLLI:        return "slli";
	case Instruction::SRLI:        return "srli";
	case Instruction::SRAI:        return "srai";
	case Instruction::ADD:         return "add";
	case Instruction::SUB:         return "sub";
	case Instruction::SLL:         return "sll";
	case Instruction::SLT:         return "slt";
	case Instruction::SLTU:        return "sltu";
	case Instruction::XOR:         return "xor";
	case Instruction::SRL:         return "srl";
	case Instruction::SRA:         return "sra";
	case Instruction::OR:          return "or";
	case Instruction::AND:         return "and";
	case Instruction::FENCE:       return "fence";
	case Instruction::ECALL:       return "ecall";
	case Instruction::EBREAK:      return "ebreak";
	case Instruction::MUL:         return "mul";
	case Instruction::MULH:        return "mulh";
	case Instruction::MULHSU:      return "mulhsu";
	case Instruction::MULHU:       return "mulhu";
	case Instruction::DIV:         return "div";
	case Instruction::DIVU:        return "divu";
	case Instruction::REM:         return "rem";
	case Instruction::REMU:        return "remu";
	case Instruction::UnknownName: return "unknown";
	}
}

std::string to_string(const reg_t& reg) {
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