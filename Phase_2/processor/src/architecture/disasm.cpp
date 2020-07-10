#ifndef __SYNTHESIS__

#include "architecture/disasm.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>

// #include "decode.hpp"
#include "architecture/architecture_helpers.hpp"
#include "architecture/decoding.hpp"
#include "debug/debug.hpp"

using namespace Decoding;

template<typename T, typename U>
bool contains(const T& container, const U& elem) {
	return std::find(std::begin(container), std::end(container), elem) != std::end(container);
}

void decode_type(DisassembledInstruction& instr) {
	const static std::array<func3_t, 6> func3IType {0b000, 0b010, 0b011, 0b100, 0b110, 0b111};

	if (instr.opcode_prefix != 0b11) {
		instr.type = Type::unknown_type;
		return;
	}

	switch (instr.opcode_suffix) {
	case Opcode::Suffix::alui:
		if (contains(func3IType, instr.func3))
			instr.type = Type::I;
		else
			instr.type = Type::R;
		break;
	case Opcode::Suffix::alu:
		instr.type = Type::R;
		break;
	case Opcode::Suffix::jalr:
	case Opcode::Suffix::load:
	case Opcode::Suffix::fence:
	case Opcode::Suffix::system:
		instr.type = Type::I;
		break;
	case Opcode::Suffix::store:
		instr.type = Type::S;
		break;
	case Opcode::Suffix::branch:
		instr.type = Type::B;
		break;
	case Opcode::Suffix::lui:
	case Opcode::Suffix::auipc:
		instr.type = Type::U;
		break;
	case Opcode::Suffix::jal:
		instr.type = Type::J;
		break;
	default:
		instr.type = Type::unknown_type;
		break;
	}
}


void decode_R_type(DisassembledInstruction& instr){
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::slli         && instr.func7 == Func7::null ) { instr.name = Name::SLLI; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::srli_srai    && instr.func7 == Func7::null ) { instr.name = Name::SRLI; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::srli_srai    && instr.func7 == Func7::sixth) { instr.name = Name::SRAI; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::add_sub_mul  && instr.func7 == Func7::null ) { instr.name = Name::ADD;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::add_sub_mul  && instr.func7 == Func7::sixth) { instr.name = Name::SUB;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::sll_mulh     && instr.func7 == Func7::null ) { instr.name = Name::SLL;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::slt_mulhsu   && instr.func7 == Func7::null ) { instr.name = Name::SLT;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::sltu_mulhu   && instr.func7 == Func7::null ) { instr.name = Name::SLTU; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::xor_div      && instr.func7 == Func7::null ) { instr.name = Name::XOR;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::srl_sra_divu && instr.func7 == Func7::null ) { instr.name = Name::SRL;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::srl_sra_divu && instr.func7 == Func7::sixth) { instr.name = Name::SRA;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::or_rem       && instr.func7 == Func7::null ) { instr.name = Name::OR;   return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::and_remu     && instr.func7 == Func7::null ) { instr.name = Name::AND;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::add_sub_mul  && instr.func7 == Func7::first) { instr.name = Name::MUL;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::sll_mulh     && instr.func7 == Func7::first) { instr.name = Name::MULH; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::slt_mulhsu   && instr.func7 == Func7::first) { instr.name = Name::MULHSU; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::sltu_mulhu   && instr.func7 == Func7::first) { instr.name = Name::MULHU; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::xor_div      && instr.func7 == Func7::first) { instr.name = Name::DIV;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::srl_sra_divu && instr.func7 == Func7::first) { instr.name = Name::DIVU; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::or_rem       && instr.func7 == Func7::first) { instr.name = Name::REM;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alu  && instr.func3 == Func3::and_remu     && instr.func7 == Func7::first) { instr.name = Name::REMU; return; }
	/*else*/ instr.name = Name::unknown_name;
}

void decode_I_type(DisassembledInstruction& instr){
	instr.immediate(11,  0) = instr.raw(31, 20);
	instr.immediate(31, 12) = instr.raw.test(31) ? -1 : 0;

	if (instr.opcode_suffix == Opcode::Suffix::fence && instr.func3 == Func3::fence) {
		instr.name = Name::FENCE;
		return;
	}
	if (instr.opcode_suffix == Opcode::Suffix::system && instr.raw(31, 7) == 0b0000000000000000000000000) {
		instr.name = Name::ECALL;
		return;
	}
	if (instr.opcode_suffix == Opcode::Suffix::system && instr.raw(31, 7) == 0b0000000000010000000000000) {
		instr.name = Name::EBREAK;
		return;
	}

	if (instr.opcode_suffix == Opcode::Suffix::jalr && instr.func3 == Func3::jalr ) { instr.name = Name::JALR; return; }
	if (instr.opcode_suffix == Opcode::Suffix::load && instr.func3 == Func3::lb   ) { instr.name = Name::LB;   return; }
	if (instr.opcode_suffix == Opcode::Suffix::load && instr.func3 == Func3::lh   ) { instr.name = Name::LH;   return; }
	if (instr.opcode_suffix == Opcode::Suffix::load && instr.func3 == Func3::lw   ) { instr.name = Name::LW;   return; }
	if (instr.opcode_suffix == Opcode::Suffix::load && instr.func3 == Func3::lbu  ) { instr.name = Name::LBU;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::load && instr.func3 == Func3::lhu  ) { instr.name = Name::LHU;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::addi ) { instr.name = Name::ADDI; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::slti ) { instr.name = Name::SLTI; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::sltiu) { instr.name = Name::SLTIU; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::xori ) { instr.name = Name::XORI; return; }
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::ori  ) { instr.name = Name::ORI;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::alui && instr.func3 == Func3::andi ) { instr.name = Name::ANDI; return; }
	/*else*/ instr.name = Name::unknown_name;
}

void decode_S_type(DisassembledInstruction& instr){
	instr.immediate( 4,  0) = instr.raw(11,  7);
	instr.immediate(11,  5) = instr.raw(31, 25);
	instr.immediate(31, 12) = instr.raw.test(31) ? -1 : 0;

	if (instr.opcode_suffix == Opcode::Suffix::store && instr.func3 == Func3::sb) { instr.name = Name::SB; return; }
	if (instr.opcode_suffix == Opcode::Suffix::store && instr.func3 == Func3::sh) { instr.name = Name::SH; return; }
	if (instr.opcode_suffix == Opcode::Suffix::store && instr.func3 == Func3::sw) { instr.name = Name::SW; return; }
	/*else*/ instr.name = Name::unknown_name;
}

void decode_B_type(DisassembledInstruction& instr){
	instr.immediate.set_bit(0, false);
	instr.immediate( 4,  1)   = instr.raw(11, 8);
	instr.immediate(10,  5)   = instr.raw(30, 25);
	instr.immediate.set_bit(11, instr.raw.get_bit(7));
	instr.immediate.set_bit(12, instr.raw.get_bit(31));
	instr.immediate(31, 13)   = instr.raw.get_bit(31) ? -1 : 0;


	if (instr.opcode_suffix == Opcode::Suffix::branch && instr.func3 == Func3::beq ) { instr.name = Name::BEQ;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::branch && instr.func3 == Func3::bne ) { instr.name = Name::BNE;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::branch && instr.func3 == Func3::blt ) { instr.name = Name::BLT;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::branch && instr.func3 == Func3::bge ) { instr.name = Name::BGE;  return; }
	if (instr.opcode_suffix == Opcode::Suffix::branch && instr.func3 == Func3::bltu) { instr.name = Name::BLTU; return; }
	if (instr.opcode_suffix == Opcode::Suffix::branch && instr.func3 == Func3::bgeu) { instr.name = Name::BGEU; return; }
	/*else*/ instr.name = Name::unknown_name;
}

void decode_U_type(DisassembledInstruction& instr){
	instr.immediate(11,  0) = 0;
	instr.immediate(31, 12) = instr.raw(31, 12);

	if (instr.opcode_suffix == Opcode::Suffix::lui)   { instr.name = Name::LUI;   return;}
	if (instr.opcode_suffix == Opcode::Suffix::auipc) { instr.name = Name::AUIPC; return;}
	/*else*/ instr.name = Name::unknown_name;
}

void decode_J_type(DisassembledInstruction& instr){
	instr.immediate.set_bit( 0, false);
	instr.immediate(10,  1)   = instr.raw(30, 21);
	instr.immediate.set_bit(11, instr.raw.get_bit(20));
	instr.immediate(19, 12)   = instr.raw(19, 12);
	instr.immediate.set_bit(20, instr.raw.get_bit(31));
	instr.immediate(31, 21)   = instr.raw.get_bit(31) ? -1 : 0;

	instr.name = Name::JAL;
}

void dispatch_decode(DisassembledInstruction& instr){
	switch (instr.type) {
	case Type::R: decode_R_type(instr); break;
	case Type::I: decode_I_type(instr); break;
	case Type::S: decode_S_type(instr); break;
	case Type::B: decode_B_type(instr); break;
	case Type::U: decode_U_type(instr); break;
	case Type::J: decode_J_type(instr); break;
	default: break;
	}
}

const DisassembledInstruction disassemble(const word_t& raw) {
	DisassembledInstruction instr;
	instr.raw           = raw;
	instr.opcode        = raw(6, 0);
	instr.opcode_prefix = raw(1, 0);
	instr.opcode_suffix = raw(6, 2);
	instr.func3         = raw(14, 12);
	instr.func7         = raw(31, 25);
	instr.destination   = raw(11,  7);
	instr.source1       = raw(19, 15);
	instr.source2       = raw(24, 20);

	decode_type(instr);
	dispatch_decode(instr);
	return instr;
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
				default:         return Name::unknown_name;
				}
			case Opcode::Suffix::Low::h00_001:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h00_010:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h00_fence:
				if (func3 == Func3::fence) return Name::FENCE;
				else                       return Name::unknown_name;
			case Opcode::Suffix::Low::h00_alui:
				switch (func3) {
				case Func3::addi:                return Name::ADDI;
				case Func3::slli:
					if      (is_func7_0b0000000) return Name::SLLI;
					else                         return Name::unknown_name;
				case Func3::slti:                return Name::SLTI;
				case Func3::sltiu:               return Name::SLTIU;
				case Func3::xori:                return Name::XORI;
				case Func3::srli_srai:
					if      (is_func7_0b0000000) return Name::SRLI;
					else if (is_func7_0b0100000) return Name::SRAI;
					else                         return Name::unknown_name;
				case Func3::ori:                 return Name::ORI;
				case Func3::andi:                return Name::ANDI;
				}
			case Opcode::Suffix::Low::h00_auipc:
				return Name::AUIPC;
			case Opcode::Suffix::Low::h00_110:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h00_111:
				return Name::unknown_name;
			}
		case Opcode::Suffix::High::store_alu_lui:
			switch (opcode_low) {
			case Opcode::Suffix::Low::h01_store:
				if (func3.test(2)) {
					return Name::unknown_name;
				} else {
					switch (func3(1, 0)) {
					case 0b00: return Name::SB;
					case 0b01: return Name::SH;
					case 0b10: return Name::SW;
					case 0b11: return Name::unknown_name;
					}
				}
			case Opcode::Suffix::Low::h01_001:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h01_010:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h01_011:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h01_alu:
				switch (func3) {
				case Func3::add_sub_mul:
					if      (is_func7_0b0000000) return Name::ADD;
					else if (is_func7_0b0100000) return Name::SUB;
					else if (is_func7_0b0000001) return Name::MUL;
					else                         return Name::unknown_name;
				case Func3::sll_mulh:
					if      (is_func7_0b0000000) return Name::SLL;
					else if (is_func7_0b0000001) return Name::MULH;
					else                         return Name::unknown_name;
				case Func3::slt_mulhsu:
					if      (is_func7_0b0000000) return Name::SLT;
					else if (is_func7_0b0000001) return Name::MULHSU;
					else                         return Name::unknown_name;
				case Func3::sltu_mulhu:
					if      (is_func7_0b0000000) return Name::SLTU;
					else if (is_func7_0b0000001) return Name::MULHU;
					else                         return Name::unknown_name;
				case Func3::xor_div:
					if      (is_func7_0b0000000) return Name::XOR;
					else if (is_func7_0b0000001) return Name::DIV;
					else                         return Name::unknown_name;
				case Func3::srl_sra_divu:
					if      (is_func7_0b0000000) return Name::SRL;
					else if (is_func7_0b0100000) return Name::SRA;
					else if (is_func7_0b0000001) return Name::DIVU;
					else                         return Name::unknown_name;
				case Func3::or_rem:
					if      (is_func7_0b0000000) return Name::OR;
					else if (is_func7_0b0000001) return Name::REM;
					else                         return Name::unknown_name;
				case Func3::and_remu:
					if      (is_func7_0b0000000) return Name::AND;
					else if (is_func7_0b0000001) return Name::REMU;
					else                         return Name::unknown_name;
				}
			case Opcode::Suffix::Low::h01_lui:
				return Name::LUI;
			case Opcode::Suffix::Low::h01_110:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h01_111:
				return Name::unknown_name;
			}
		case Opcode::Suffix::High::h10_unused:
			return Name::unknown_name;
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
				default:          return Name::unknown_name;
				}
			case Opcode::Suffix::Low::h11_jalr:
				if (func3 == Func3::jalr) {
					return Name::JALR;
				}
				else {
					return Name::unknown_name;
				}
			case Opcode::Suffix::Low::h11_010:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h11_jal:
				return Name::JAL;
			case Opcode::Suffix::Low::h11_system: {
				const ap_uint<25> no_opcode = instruction(31, 7);
				if      (no_opcode == 0b0000000000000000000000000) return Name::EBREAK;
				else if (no_opcode == 0b0000000000010000000000000) return Name::ECALL;
				else                                               return Name::unknown_name;
				}
			case Opcode::Suffix::Low::h11_101:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h11_110:
				return Name::unknown_name;
			case Opcode::Suffix::Low::h11_111:
				return Name::unknown_name;
			}
		}
	} else {
		return Name::unknown_name;
	}
}

std::string instruction_to_string(const DisassembledInstruction& instr) {
	std::ostringstream ss;
	switch (instr.type) {
	case Type::R:
		if (	instr.name == Name::SLLI
			||	instr.name == Name::SRLI
			||	instr.name == Name::SRAI)
		{
			ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << to_string(instr.source1) << ", " << instr.immediate(4, 0).to_uint();
		}
		else {
			ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << to_string(instr.source1) << ", " << to_string(instr.source2);
		}
		break;
	case Type::I:
		if (instr.opcode_suffix == Opcode::Suffix::jalr || instr.opcode_suffix == Opcode::Suffix::load)
			ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << instr.immediate.to_int() << "(" << to_string(instr.source1) << ")";
		else
			ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << to_string(instr.source1) << ", " << instr.immediate.to_int();
		break;
	case Type::S:
		ss << to_string(instr.name) << ' ' << to_string(instr.source1) << ", " << instr.immediate.to_int() << '(' << to_string(instr.source2) << ')';
		break;
	case Type::B:
		ss << to_string(instr.name) << ' ' << to_string(instr.source1) << ", " << to_string(instr.source2) << ", " << instr.immediate.to_int();
		break;
	case Type::U:
		ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << instr.immediate.to_int();
		break;
	case Type::J:
		ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << instr.immediate.to_int();
		break;
	default:
		ss << "# Unknown instruction";
		break;
	}
	return ss.str();
}

json instruction_to_json(const DisassembledInstruction& instr) {
	switch (instr.type) {
	case Type::R:
		if (	instr.name == Name::SLLI
			||	instr.name == Name::SRLI
			||	instr.name == Name::SRAI)
		{
			return {
				{ "Type", to_string(instr.type) },
				{ "Name", to_string(instr.name) },
				{ "Destination register", to_string(instr.destination) },
				{ "Source 1 register", to_string(instr.source1) },
				{ "Shift amount", instr.immediate(4, 0).to_uint() },
			};
		}
		else {
			return {
				{ "Type", to_string(instr.type) },
				{ "Name", to_string(instr.name) },
				{ "Destination register", to_string(instr.destination) },
				{ "Source 1 register", to_string(instr.source1) },
				{ "Source 2 register", to_string(instr.source2) },
			};
		}
	case Type::I:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Destination register", to_string(instr.destination) },
			{ "Source 1 register", to_string(instr.source1) },
			{ "Immediate", instr.immediate.to_int() },
		};
	case Type::S:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Source 1 register", to_string(instr.source1) },
			{ "Source 2 register", to_string(instr.source2) },
			{ "Immediate", instr.immediate.to_int() },
		};
	case Type::B:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Source 1 register", to_string(instr.source1) },
			{ "Source 2 register", to_string(instr.source2) },
			{ "Immediate", instr.immediate.to_int() },
		};
	case Type::U:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Destination register", to_string(instr.destination) },
			{ "Immediate", instr.immediate.to_int() },
		};
	case Type::J:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Destination register", to_string(instr.destination) },
			{ "Immediate", instr.immediate.to_int() },
		};
	default:
		return {
			{ "Type", to_string(instr.type) },
			{ "Opcode", string_bin(instr.opcode.to_uint()) },
			{ "Raw", string_hex(instr.raw.to_uint()) },
		};
	}
}

#endif // __SYNTHESIS__