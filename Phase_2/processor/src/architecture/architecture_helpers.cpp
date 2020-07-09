#include "architecture/architecture_helpers.hpp"

#include "architecture/decoding.hpp"

std::string to_string(Type type) {
	switch (type) {
	case Type::R: return "R";
	case Type::I: return "I";
	case Type::S: return "S";
	case Type::B: return "B";
	case Type::U: return "U";
	case Type::J: return "J";
	case Type::unknown: return "Unknown";
	default: return "Unknown";
	}
}

using namespace Decoding;
std::string to_string(Kind kind) {
	switch (kind) {
	case Kind::alu:           return "alu";
	case Kind::alu_immediate: return "alu_immediate";
	case Kind::load:          return "load";
	case Kind::store:         return "store";
	case Kind::branch:        return "branch";
	case Kind::other:         return "other";
	default: return "Unknown";
	}
}

std::string to_string(Name name) {
	switch (name) {
	case Name::LUI:     return "lui";
	case Name::AUIPC:   return "auipc";
	case Name::JAL:     return "jal";
	case Name::JALR:    return "jalr";
	case Name::BEQ:     return "beq";
	case Name::BNE:     return "bne";
	case Name::BLT:     return "blt";
	case Name::BGE:     return "bge";
	case Name::BLTU:    return "bltu";
	case Name::BGEU:    return "bgeu";
	case Name::LB:      return "lb";
	case Name::LH:      return "lh";
	case Name::LW:      return "lw";
	case Name::LBU:     return "lbu";
	case Name::LHU:     return "lhu";
	case Name::SB:      return "sb";
	case Name::SH:      return "sh";
	case Name::SW:      return "sw";
	case Name::ADDI:    return "addi";
	case Name::SLTI:    return "slti";
	case Name::SLTIU:   return "sltiu";
	case Name::XORI:    return "xori";
	case Name::ORI:     return "ori";
	case Name::ANDI:    return "andi";
	case Name::SLLI:    return "slli";
	case Name::SRLI:    return "srli";
	case Name::SRAI:    return "srai";
	case Name::ADD:     return "add";
	case Name::SUB:     return "sub";
	case Name::SLL:     return "sll";
	case Name::SLT:     return "slt";
	case Name::SLTU:    return "sltu";
	case Name::XOR:     return "xor";
	case Name::SRL:     return "srl";
	case Name::SRA:     return "sra";
	case Name::OR:      return "or";
	case Name::AND:     return "and";
	case Name::FENCE:   return "fence";
	case Name::ECALL:   return "ecall";
	case Name::EBREAK:  return "ebreak";
	case Name::MUL:     return "mul";
	case Name::MULH:    return "mulh";
	case Name::MULHSU:  return "mulhsu";
	case Name::MULHU:   return "mulhu";
	case Name::DIV:     return "div";
	case Name::DIVU:    return "divu";
	case Name::REM:     return "rem";
	case Name::REMU:    return "remu";
	case Name::unknown: return "unknown";
	default: return "unknown";
	}
}

std::string to_string(const reg_t& reg, bool nice_names) {
	switch (static_cast<Register>(reg.to_uint())) {
	case Register::x0 : return nice_names ? "zero" :  "x0";
	case Register::x1 : return nice_names ?   "ra" :  "x1";
	case Register::x2 : return nice_names ?   "sp" :  "x2";
	case Register::x3 : return nice_names ?   "gp" :  "x3";
	case Register::x4 : return nice_names ?   "tp" :  "x4";
	case Register::x5 : return nice_names ?   "t0" :  "x5";
	case Register::x6 : return nice_names ?   "t1" :  "x6";
	case Register::x7 : return nice_names ?   "t2" :  "x7";
	case Register::x8 : return nice_names ?   "s0" :  "x8";
	case Register::x9 : return nice_names ?   "s1" :  "x9";
	case Register::x10: return nice_names ?   "a0" : "x10";
	case Register::x11: return nice_names ?   "a1" : "x11";
	case Register::x12: return nice_names ?   "a2" : "x12";
	case Register::x13: return nice_names ?   "a3" : "x13";
	case Register::x14: return nice_names ?   "a4" : "x14";
	case Register::x15: return nice_names ?   "a5" : "x15";
	case Register::x16: return nice_names ?   "a6" : "x16";
	case Register::x17: return nice_names ?   "a7" : "x17";
	case Register::x18: return nice_names ?   "s2" : "x18";
	case Register::x19: return nice_names ?   "s3" : "x19";
	case Register::x20: return nice_names ?   "s4" : "x20";
	case Register::x21: return nice_names ?   "s5" : "x21";
	case Register::x22: return nice_names ?   "s6" : "x22";
	case Register::x23: return nice_names ?   "s7" : "x23";
	case Register::x24: return nice_names ?   "s8" : "x24";
	case Register::x25: return nice_names ?   "s9" : "x25";
	case Register::x26: return nice_names ?  "s10" : "x26";
	case Register::x27: return nice_names ?  "s11" : "x27";
	case Register::x28: return nice_names ?   "t3" : "x28";
	case Register::x29: return nice_names ?   "t4" : "x29";
	case Register::x30: return nice_names ?   "t5" : "x30";
	case Register::x31: return nice_names ?   "t6" : "x31";
	default: return "Unknown";
	}
}