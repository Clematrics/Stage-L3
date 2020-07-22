#include "architecture/architecture_helpers.hpp"

#include "architecture/decoding.hpp"

std::string to_string(Type type) {
	switch (type) {
	case Architecture::Type::R: return "R";
	case Architecture::Type::I: return "I";
	case Architecture::Type::S: return "S";
	case Architecture::Type::B: return "B";
	case Architecture::Type::U: return "U";
	case Architecture::Type::J: return "J";
	default: return "Unknown";
	}
}

std::string to_string(Name name) {
	switch (name) {
	case Architecture::Name::LUI:     return "lui";
	case Architecture::Name::AUIPC:   return "auipc";
	case Architecture::Name::JAL:     return "jal";
	case Architecture::Name::JALR:    return "jalr";
	case Architecture::Name::BEQ:     return "beq";
	case Architecture::Name::BNE:     return "bne";
	case Architecture::Name::BLT:     return "blt";
	case Architecture::Name::BGE:     return "bge";
	case Architecture::Name::BLTU:    return "bltu";
	case Architecture::Name::BGEU:    return "bgeu";
	case Architecture::Name::LB:      return "lb";
	case Architecture::Name::LH:      return "lh";
	case Architecture::Name::LW:      return "lw";
	case Architecture::Name::LBU:     return "lbu";
	case Architecture::Name::LHU:     return "lhu";
	case Architecture::Name::SB:      return "sb";
	case Architecture::Name::SH:      return "sh";
	case Architecture::Name::SW:      return "sw";
	case Architecture::Name::ADDI:    return "addi";
	case Architecture::Name::SLTI:    return "slti";
	case Architecture::Name::SLTIU:   return "sltiu";
	case Architecture::Name::XORI:    return "xori";
	case Architecture::Name::ORI:     return "ori";
	case Architecture::Name::ANDI:    return "andi";
	case Architecture::Name::SLLI:    return "slli";
	case Architecture::Name::SRLI:    return "srli";
	case Architecture::Name::SRAI:    return "srai";
	case Architecture::Name::ADD:     return "add";
	case Architecture::Name::SUB:     return "sub";
	case Architecture::Name::SLL:     return "sll";
	case Architecture::Name::SLT:     return "slt";
	case Architecture::Name::SLTU:    return "sltu";
	case Architecture::Name::XOR:     return "xor";
	case Architecture::Name::SRL:     return "srl";
	case Architecture::Name::SRA:     return "sra";
	case Architecture::Name::OR:      return "or";
	case Architecture::Name::AND:     return "and";
	case Architecture::Name::FENCE:   return "fence";
	case Architecture::Name::ECALL:   return "ecall";
	case Architecture::Name::EBREAK:  return "ebreak";
	case Architecture::Name::MUL:     return "mul";
	case Architecture::Name::MULH:    return "mulh";
	case Architecture::Name::MULHSU:  return "mulhsu";
	case Architecture::Name::MULHU:   return "mulhu";
	case Architecture::Name::DIV:     return "div";
	case Architecture::Name::DIVU:    return "divu";
	case Architecture::Name::REM:     return "rem";
	case Architecture::Name::REMU:    return "remu";
	default: return "unknown";
	}
}

std::string to_string(const reg_t& reg, bool nice_names) {
	switch (static_cast<Register>(reg.to_uint())) {
	case Architecture::Register::x0 : return nice_names ? "zero" :  "x0";
	case Architecture::Register::x1 : return nice_names ?   "ra" :  "x1";
	case Architecture::Register::x2 : return nice_names ?   "sp" :  "x2";
	case Architecture::Register::x3 : return nice_names ?   "gp" :  "x3";
	case Architecture::Register::x4 : return nice_names ?   "tp" :  "x4";
	case Architecture::Register::x5 : return nice_names ?   "t0" :  "x5";
	case Architecture::Register::x6 : return nice_names ?   "t1" :  "x6";
	case Architecture::Register::x7 : return nice_names ?   "t2" :  "x7";
	case Architecture::Register::x8 : return nice_names ?   "s0" :  "x8";
	case Architecture::Register::x9 : return nice_names ?   "s1" :  "x9";
	case Architecture::Register::x10: return nice_names ?   "a0" : "x10";
	case Architecture::Register::x11: return nice_names ?   "a1" : "x11";
	case Architecture::Register::x12: return nice_names ?   "a2" : "x12";
	case Architecture::Register::x13: return nice_names ?   "a3" : "x13";
	case Architecture::Register::x14: return nice_names ?   "a4" : "x14";
	case Architecture::Register::x15: return nice_names ?   "a5" : "x15";
	case Architecture::Register::x16: return nice_names ?   "a6" : "x16";
	case Architecture::Register::x17: return nice_names ?   "a7" : "x17";
	case Architecture::Register::x18: return nice_names ?   "s2" : "x18";
	case Architecture::Register::x19: return nice_names ?   "s3" : "x19";
	case Architecture::Register::x20: return nice_names ?   "s4" : "x20";
	case Architecture::Register::x21: return nice_names ?   "s5" : "x21";
	case Architecture::Register::x22: return nice_names ?   "s6" : "x22";
	case Architecture::Register::x23: return nice_names ?   "s7" : "x23";
	case Architecture::Register::x24: return nice_names ?   "s8" : "x24";
	case Architecture::Register::x25: return nice_names ?   "s9" : "x25";
	case Architecture::Register::x26: return nice_names ?  "s10" : "x26";
	case Architecture::Register::x27: return nice_names ?  "s11" : "x27";
	case Architecture::Register::x28: return nice_names ?   "t3" : "x28";
	case Architecture::Register::x29: return nice_names ?   "t4" : "x29";
	case Architecture::Register::x30: return nice_names ?   "t5" : "x30";
	case Architecture::Register::x31: return nice_names ?   "t6" : "x31";
	default: return "Unknown";
	}
}