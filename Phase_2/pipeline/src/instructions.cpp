#include "../include/instructions.hpp"

std::string to_string(Instruction::Type type) {
	switch (type) {
	case Instruction::Type::R: return "R";
	case Instruction::Type::I: return "I";
	case Instruction::Type::S: return "S";
	case Instruction::Type::B: return "B";
	case Instruction::Type::U: return "U";
	case Instruction::Type::J: return "J";
	case Instruction::Type::UnknownType: return "Unknown";
	}
}

std::string to_string(Instruction::Name name) {
	switch (name) {
	case Instruction::Name::LUI:         return "lui";
	case Instruction::Name::AUIPC:       return "auipc";
	case Instruction::Name::JAL:         return "jal";
	case Instruction::Name::JALR:        return "jalr";
	case Instruction::Name::BEQ:         return "beq";
	case Instruction::Name::BNE:         return "bne";
	case Instruction::Name::BLT:         return "blt";
	case Instruction::Name::BGE:         return "bge";
	case Instruction::Name::BLTU:        return "bltu";
	case Instruction::Name::BGEU:        return "bgeu";
	case Instruction::Name::LB:          return "lb";
	case Instruction::Name::LH:          return "lh";
	case Instruction::Name::LW:          return "lw";
	case Instruction::Name::LBU:         return "lbu";
	case Instruction::Name::LHU:         return "lhu";
	case Instruction::Name::SB:          return "sb";
	case Instruction::Name::SH:          return "sh";
	case Instruction::Name::SW:          return "sw";
	case Instruction::Name::ADDI:        return "addi";
	case Instruction::Name::SLTI:        return "slti";
	case Instruction::Name::SLTIU:       return "sltiu";
	case Instruction::Name::XORI:        return "xori";
	case Instruction::Name::ORI:         return "ori";
	case Instruction::Name::ANDI:        return "andi";
	case Instruction::Name::SLLI:        return "slli";
	case Instruction::Name::SRLI:        return "srli";
	case Instruction::Name::SRAI:        return "srai";
	case Instruction::Name::ADD:         return "add";
	case Instruction::Name::SUB:         return "sub";
	case Instruction::Name::SLL:         return "sll";
	case Instruction::Name::SLT:         return "slt";
	case Instruction::Name::SLTU:        return "sltu";
	case Instruction::Name::XOR:         return "xor";
	case Instruction::Name::SRL:         return "srl";
	case Instruction::Name::SRA:         return "sra";
	case Instruction::Name::OR:          return "or";
	case Instruction::Name::AND:         return "and";
	case Instruction::Name::FENCE:       return "fence";
	case Instruction::Name::ECALL:       return "ecall";
	case Instruction::Name::EBREAK:      return "ebreak";
	case Instruction::Name::MUL:         return "mul";
	case Instruction::Name::MULH:        return "mulh";
	case Instruction::Name::MULHSU:      return "mulhsu";
	case Instruction::Name::MULHU:       return "mulhu";
	case Instruction::Name::DIV:         return "div";
	case Instruction::Name::DIVU:        return "divu";
	case Instruction::Name::REM:         return "rem";
	case Instruction::Name::REMU:        return "remu";
	case Instruction::Name::UnknownName: return "unknown";
	}
}

std::string to_string(reg_t reg) {
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