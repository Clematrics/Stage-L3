#include "../include/disasm.hpp"

#ifndef __SYNTHESIS__

#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>

#include "../include/helpers.hpp"
#include "../include/instructions.hpp"
#include "../include/json.hpp"


template<typename T, typename U>
bool contains(const T& container, const U& elem) {
	return std::find(std::begin(container), std::end(container), elem) != std::end(container);
}

void decodeType(AsmInstruction& instr) {
	const static std::array<func3_t, 6> func3IType {0b000, 0b010, 0b011, 0b100, 0b110, 0b111};

	if (instr.opcode_prefix != 0b11) {
		instr.type = Instruction::Type::UnknownType;
		return;
	}

	switch (instr.opcode_suffix) {
	case OPCODE_SUFFIX_ALUI:
		if (contains(func3IType, instr.func3))
			instr.type = Instruction::Type::I;
		else
			instr.type = Instruction::Type::R;
		break;
	case OPCODE_SUFFIX_ALU:
		instr.type = Instruction::Type::R;
		break;
	case OPCODE_SUFFIX_JALR:
	case OPCODE_SUFFIX_LOAD:
	case OPCODE_SUFFIX_FENCE:
	case OPCODE_SUFFIX_SYSTEM:
		instr.type = Instruction::Type::I;
		break;
	case OPCODE_SUFFIX_STORE:
		instr.type = Instruction::Type::S;
		break;
	case OPCODE_SUFFIX_BRANCH:
		instr.type = Instruction::Type::B;
		break;
	case OPCODE_SUFFIX_LUI:
	case OPCODE_SUFFIX_AUIPC:
		instr.type = Instruction::Type::U;
		break;
	case OPCODE_SUFFIX_JAL:
		instr.type = Instruction::Type::J;
		break;
	default:
		instr.type = Instruction::Type::UnknownType;
		break;
	}
}


void decodeRType(AsmInstruction& instr){
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_SLLI         && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::SLLI; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_SRLI_SRAI    && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::SRLI; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_SRLI_SRAI    && instr.func7 == FUNC7_SIXTH) { instr.name = Instruction::Name::SRAI; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_ADD_SUB_MUL  && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::ADD;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_ADD_SUB_MUL  && instr.func7 == FUNC7_SIXTH) { instr.name = Instruction::Name::SUB;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SLL_MULH     && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::SLL;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SLT_MULHSU   && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::SLT;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SLTU_MULHU   && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::SLTU; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_XOR_DIV      && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::XOR;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SRL_SRA_DIVU && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::SRL;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SRL_SRA_DIVU && instr.func7 == FUNC7_SIXTH) { instr.name = Instruction::Name::SRA;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_OR_REM       && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::OR;   return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_AND_REMU     && instr.func7 == FUNC7_NULL ) { instr.name = Instruction::Name::AND;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_ADD_SUB_MUL  && instr.func7 == FUNC7_FIRST) { instr.name = Instruction::Name::MUL;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SLL_MULH     && instr.func7 == FUNC7_FIRST) { instr.name = Instruction::Name::MULH; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SLT_MULHSU   && instr.func7 == FUNC7_FIRST) { instr.name = Instruction::Name::MULHSU; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SLTU_MULHU   && instr.func7 == FUNC7_FIRST) { instr.name = Instruction::Name::MULHU; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_XOR_DIV      && instr.func7 == FUNC7_FIRST) { instr.name = Instruction::Name::DIV;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_SRL_SRA_DIVU && instr.func7 == FUNC7_FIRST) { instr.name = Instruction::Name::DIVU; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_OR_REM       && instr.func7 == FUNC7_FIRST) { instr.name = Instruction::Name::REM;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALU  && instr.func3 == FUNC3_AND_REMU     && instr.func7 == FUNC7_FIRST) { instr.name = Instruction::Name::REMU; return; }
	/*else*/ instr.name = Instruction::Name::UnknownName;
}

void decodeIType(AsmInstruction& instr){
	instr.immediate(11,  0) = instr.raw(31, 20);
	instr.immediate(31, 12) = instr.raw.test(31) ? -1 : 0;

	if (instr.opcode_suffix == OPCODE_SUFFIX_FENCE && instr.func3 == FUNC3_FENCE) {
		instr.name = Instruction::Name::FENCE;
		return;
	}
	if (instr.opcode_suffix == OPCODE_SUFFIX_SYSTEM && instr.raw(31, 7) == 0b0000000000000000000000000) {
		instr.name = Instruction::Name::ECALL;
		return;
	}
	if (instr.opcode_suffix == OPCODE_SUFFIX_SYSTEM && instr.raw(31, 7) == 0b0000000000010000000000000) {
		instr.name = Instruction::Name::EBREAK;
		return;
	}

	if (instr.opcode_suffix == OPCODE_SUFFIX_JALR && instr.func3 == FUNC3_JALR ) { instr.name = Instruction::Name::JALR; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_LOAD && instr.func3 == FUNC3_LB   ) { instr.name = Instruction::Name::LB;   return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_LOAD && instr.func3 == FUNC3_LH   ) { instr.name = Instruction::Name::LH;   return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_LOAD && instr.func3 == FUNC3_LW   ) { instr.name = Instruction::Name::LW;   return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_LOAD && instr.func3 == FUNC3_LBU  ) { instr.name = Instruction::Name::LBU;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_LOAD && instr.func3 == FUNC3_LHU  ) { instr.name = Instruction::Name::LHU;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_ADDI ) { instr.name = Instruction::Name::ADDI; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_SLTI ) { instr.name = Instruction::Name::SLTI; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_SLTIU) { instr.name = Instruction::Name::SLTIU; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_XORI ) { instr.name = Instruction::Name::XORI; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_ORI  ) { instr.name = Instruction::Name::ORI;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_ALUI && instr.func3 == FUNC3_ANDI ) { instr.name = Instruction::Name::ANDI; return; }
	/*else*/ instr.name = Instruction::Name::UnknownName;
}

void decodeSType(AsmInstruction& instr){
	instr.immediate( 4,  0) = instr.raw(11,  7);
	instr.immediate(11,  5) = instr.raw(31, 25);
	instr.immediate(31, 12) = instr.raw.test(31) ? -1 : 0;

	if (instr.opcode_suffix == OPCODE_SUFFIX_STORE && instr.func3 == FUNC3_SB) { instr.name = Instruction::Name::SB; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_STORE && instr.func3 == FUNC3_SH) { instr.name = Instruction::Name::SH; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_STORE && instr.func3 == FUNC3_SW) { instr.name = Instruction::Name::SW; return; }
	/*else*/ instr.name = Instruction::Name::UnknownName;
}

void decodeBType(AsmInstruction& instr){
	instr.immediate.set_bit(0, false);
	instr.immediate( 4,  1)   = instr.raw(11, 8);
	instr.immediate(10,  5)   = instr.raw(30, 25);
	instr.immediate.set_bit(11, instr.raw.get_bit(7));
	instr.immediate.set_bit(12, instr.raw.get_bit(31));
	instr.immediate(31, 13)   = instr.raw.get_bit(31) ? -1 : 0;


	if (instr.opcode_suffix == OPCODE_SUFFIX_BRANCH && instr.func3 == FUNC3_BEQ ) { instr.name = Instruction::Name::BEQ;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_BRANCH && instr.func3 == FUNC3_BNE ) { instr.name = Instruction::Name::BNE;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_BRANCH && instr.func3 == FUNC3_BLT ) { instr.name = Instruction::Name::BLT;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_BRANCH && instr.func3 == FUNC3_BGE ) { instr.name = Instruction::Name::BGE;  return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_BRANCH && instr.func3 == FUNC3_BLTU) { instr.name = Instruction::Name::BLTU; return; }
	if (instr.opcode_suffix == OPCODE_SUFFIX_BRANCH && instr.func3 == FUNC3_BGEU) { instr.name = Instruction::Name::BGEU; return; }
	/*else*/ instr.name = Instruction::Name::UnknownName;
}

void decodeUType(AsmInstruction& instr){
	instr.immediate(11,  0) = 0;
	instr.immediate(31, 12) = instr.raw(31, 12);

	if (instr.opcode_suffix == OPCODE_SUFFIX_LUI)   { instr.name = Instruction::Name::LUI;   return;}
	if (instr.opcode_suffix == OPCODE_SUFFIX_AUIPC) { instr.name = Instruction::Name::AUIPC; return;}
	/*else*/ instr.name = Instruction::Name::UnknownName;
}

void decodeJType(AsmInstruction& instr){
	instr.immediate.set_bit( 0, false);
	instr.immediate(10,  1)   = instr.raw(30, 21);
	instr.immediate.set_bit(11, instr.raw.get_bit(20));
	instr.immediate(19, 12)   = instr.raw(19, 12);
	instr.immediate.set_bit(20, instr.raw.get_bit(31));
	instr.immediate(31, 21)   = instr.raw.get_bit(31) ? -1 : 0;

	instr.name = Instruction::Name::JAL;
}

void dispatchDecode(AsmInstruction& instr){
	switch (instr.type) {
	case Instruction::Type::R: decodeRType(instr); break;
	case Instruction::Type::I: decodeIType(instr); break;
	case Instruction::Type::S: decodeSType(instr); break;
	case Instruction::Type::B: decodeBType(instr); break;
	case Instruction::Type::U: decodeUType(instr); break;
	case Instruction::Type::J: decodeJType(instr); break;
	default: break;
	}
}

const AsmInstruction disassemble(const word_t& raw) {
	AsmInstruction instr;
	instr.raw           = raw;
	instr.opcode        = raw(6, 0);
	instr.opcode_prefix = raw(1, 0);
	instr.opcode_suffix = raw(6, 2);
	instr.func3         = raw(14, 12);
	instr.func7         = raw(31, 25);
	instr.destination   = raw(11,  7);
	instr.source1       = raw(19, 15);
	instr.source2       = raw(24, 20);

	decodeType(instr);
	dispatchDecode(instr);
	return instr;
}

std::string instruction_to_string(const AsmInstruction& instr) {
	std::ostringstream ss;
	switch (instr.type) {
	case Instruction::Type::R:
		if (	instr.name == Instruction::Name::SLLI
			||	instr.name == Instruction::Name::SRLI
			||	instr.name == Instruction::Name::SRAI)
		{
			ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << to_string(instr.source1) << ", " << instr.immediate(4, 0).to_uint();
		}
		else {
			ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << to_string(instr.source1) << ", " << to_string(instr.source2);
		}
		break;
	case Instruction::Type::I:
		if (instr.opcode_suffix == OPCODE_SUFFIX_JALR || instr.opcode_suffix == OPCODE_SUFFIX_LOAD)
			ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << instr.immediate.to_int() << "(" << to_string(instr.source1) << ")";
		else
			ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << to_string(instr.source1) << ", " << instr.immediate.to_int();
		break;
	case Instruction::Type::S:
		ss << to_string(instr.name) << ' ' << to_string(instr.source1) << ", " << instr.immediate.to_int() << '(' << to_string(instr.source2) << ')';
		break;
	case Instruction::Type::B:
		ss << to_string(instr.name) << ' ' << to_string(instr.source1) << ", " << to_string(instr.source2) << ", " << instr.immediate.to_int();
		break;
	case Instruction::Type::U:
		ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << instr.immediate.to_int();
		break;
	case Instruction::Type::J:
		ss << to_string(instr.name) << ' ' << to_string(instr.destination) << ", " << instr.immediate.to_int();
		break;
	default:
		ss << "# Unknown instruction";
		break;
	}
	return ss.str();
}

json instruction_to_json(const AsmInstruction& instr) {
	switch (instr.type) {
	case Instruction::Type::R:
		if (	instr.name == Instruction::Name::SLLI
			||	instr.name == Instruction::Name::SRLI
			||	instr.name == Instruction::Name::SRAI)
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
	case Instruction::Type::I:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Destination register", to_string(instr.destination) },
			{ "Source 1 register", to_string(instr.source1) },
			{ "Immediate", instr.immediate.to_int() },
		};
	case Instruction::Type::S:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Source 1 register", to_string(instr.source1) },
			{ "Source 2 register", to_string(instr.source2) },
			{ "Immediate", instr.immediate.to_int() },
		};
	case Instruction::Type::B:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Source 1 register", to_string(instr.source1) },
			{ "Source 2 register", to_string(instr.source2) },
			{ "Immediate", instr.immediate.to_int() },
		};
	case Instruction::Type::U:
		return {
			{ "Type", to_string(instr.type) },
			{ "Name", to_string(instr.name) },
			{ "Destination register", to_string(instr.destination) },
			{ "Immediate", instr.immediate.to_int() },
		};
	case Instruction::Type::J:
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