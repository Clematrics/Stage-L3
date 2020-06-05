#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <iterator>

#include "decode.hpp"

template<typename T, typename U>
bool contains(const T& container, const U& elem) {
	return std::find(std::begin(container), std::end(container), elem) != std::end(container);
}

void getType(Instruction& instr) {
	const static std::array<uint8_t, 6> func3IType {0b000, 0b010, 0b011, 0b100, 0b110, 0b111};
	uint8_t func3 = (instr.raw & 0x00007000) >> 12;

	switch (instr.opcode) {
	case Opcode::ALUI:
		if (contains(func3IType, func3))
			instr.type = InstructionType::I;
		else
			instr.type = InstructionType::R;
		break;
	case Opcode::ALU:
		instr.type = InstructionType::R;
		break;
	case Opcode::JALR:
	case Opcode::LOAD:
	case Opcode::FENCE:
	case Opcode::SYS:
		instr.type = InstructionType::I;
		break;
	case Opcode::STORE:
		instr.type = InstructionType::S;
		break;
	case Opcode::BRANCH:
		instr.type = InstructionType::B;
		break;
	case Opcode::LUI:
	case Opcode::AUIPC:
		instr.type = InstructionType::U;
		break;
	case Opcode::JAL:
		instr.type = InstructionType::J;
		break;
	default:
		instr.type = InstructionType::Unknown;
		break;
	}
}

void decodeRType(Instruction& instr){
	uint32_t destination 	= (instr.raw & 0x00000F80) >>  7;
	uint32_t func3 			= (instr.raw & 0x00007000) >> 12;
	uint32_t source1 		= (instr.raw & 0x000F8000) >> 15;
	uint32_t source2 		= (instr.raw & 0x01F00000) >> 20;
	uint32_t func7 			= (instr.raw & 0xFE000000) >> 25;

	instr.destination	= static_cast<Register>(destination);
	instr.source1 		= static_cast<Register>(source1);
	instr.source2 		= static_cast<Register>(source2);

	instr.func3 = func3;
	instr.func7 = func7;

	if (instr.opcode == Opcode::ALUI && func3 == 0b001 && func7 == 0b0000000) { instr.name = InstructionName::SLLI;	instr.shift_amount = source2; return; }
	if (instr.opcode == Opcode::ALUI && func3 == 0b101 && func7 == 0b0000000) { instr.name = InstructionName::SRLI; instr.shift_amount = source2; return; }
	if (instr.opcode == Opcode::ALUI && func3 == 0b101 && func7 == 0b0100000) { instr.name = InstructionName::SRAI; instr.shift_amount = source2; return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b000 && func7 == 0b0000000) { instr.name = InstructionName::ADD; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b000 && func7 == 0b0100000) { instr.name = InstructionName::SUB; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b001 && func7 == 0b0000000) { instr.name = InstructionName::SLL; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b010 && func7 == 0b0000000) { instr.name = InstructionName::SLT; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b011 && func7 == 0b0000000) { instr.name = InstructionName::SLTU; return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b100 && func7 == 0b0000000) { instr.name = InstructionName::XOR; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b101 && func7 == 0b0000000) { instr.name = InstructionName::SRL; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b101 && func7 == 0b0100000) { instr.name = InstructionName::SRA; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b110 && func7 == 0b0000000) { instr.name = InstructionName::OR; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b111 && func7 == 0b0000000) { instr.name = InstructionName::AND; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b000 && func7 == 0b0000001) { instr.name = InstructionName::MUL; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b001 && func7 == 0b0000001) { instr.name = InstructionName::MULH; return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b010 && func7 == 0b0000001) { instr.name = InstructionName::MULHSU; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b011 && func7 == 0b0000001) { instr.name = InstructionName::MULHU; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b100 && func7 == 0b0000001) { instr.name = InstructionName::DIV; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b101 && func7 == 0b0000001) { instr.name = InstructionName::DIVU; return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b110 && func7 == 0b0000001) { instr.name = InstructionName::REM; 	return; }
	if (instr.opcode == Opcode::ALU  && func3 == 0b111 && func7 == 0b0000001) { instr.name = InstructionName::REMU; return; }
	/*else*/ instr.name = InstructionName::Unknown;
}

void decodeIType(Instruction& instr){
	uint32_t destination 	= (instr.raw & 0x00000F80) >>  7;
	uint32_t func3 			= (instr.raw & 0x00007000) >> 12;
	uint32_t source1 		= (instr.raw & 0x000F8000) >> 15;
	uint32_t imm_11_0 		= (instr.raw & 0xFFF00000) >> 20;

	instr.destination	= static_cast<Register>(destination);
	instr.source1 		= static_cast<Register>(source1);
	instr.immediate		= { imm_11_0, 0x00000FFF };

	if (instr.opcode == Opcode::FENCE && func3 == 0b000) {
		instr.name = InstructionName::FENCE;
		return;
	}
	if (instr.opcode == Opcode::SYS && instr.raw == 0b000000000000'00000'000'00000'1110011) {
		instr.name = InstructionName::ECALL;
		return;
	}
	if (instr.opcode == Opcode::SYS && instr.raw == 0b000000000001'00000'000'00000'1110011) {
		instr.name = InstructionName::EBREAK;
		return;
	}

	if (instr.opcode == Opcode::JALR && func3 == 0b000) { instr.name = InstructionName::JALR; 	return; }
	if (instr.opcode == Opcode::LOAD && func3 == 0b000) { instr.name = InstructionName::LB; 	return; }
	if (instr.opcode == Opcode::LOAD && func3 == 0b001) { instr.name = InstructionName::LH; 	return; }
	if (instr.opcode == Opcode::LOAD && func3 == 0b010) { instr.name = InstructionName::LW; 	return; }
	if (instr.opcode == Opcode::LOAD && func3 == 0b100) { instr.name = InstructionName::LBU; 	return; }
	if (instr.opcode == Opcode::LOAD && func3 == 0b101) { instr.name = InstructionName::LHU; 	return; }
	if (instr.opcode == Opcode::ALUI && func3 == 0b000) { instr.name = InstructionName::ADDI; 	return; }
	if (instr.opcode == Opcode::ALUI && func3 == 0b010) { instr.name = InstructionName::SLTI; 	return; }
	if (instr.opcode == Opcode::ALUI && func3 == 0b011) { instr.name = InstructionName::SLTIU; 	return; }
	if (instr.opcode == Opcode::ALUI && func3 == 0b100) { instr.name = InstructionName::XORI; 	return; }
	if (instr.opcode == Opcode::ALUI && func3 == 0b110) { instr.name = InstructionName::ORI; 	return; }
	if (instr.opcode == Opcode::ALUI && func3 == 0b111) { instr.name = InstructionName::ANDI; 	return; }
	/*else*/ instr.name = InstructionName::Unknown;
}

void decodeSType(Instruction& instr){
	uint32_t imm_4_0 	= (instr.raw & 0x00000F80) >>  7;
	uint32_t func3 		= (instr.raw & 0x00007000) >> 12;
	uint32_t source1 	= (instr.raw & 0x000F8000) >> 15;
	uint32_t source2 	= (instr.raw & 0x01F00000) >> 20;
	uint32_t imm_11_5 	= (instr.raw & 0xFE000000) >> 25;

	instr.func3 		= func3;
	instr.source1 		= static_cast<Register>(source1);
	instr.source2 		= static_cast<Register>(source2);
	instr.immediate		= { (imm_11_5 << 5) + imm_4_0, 0x00000FFF };


	if (instr.opcode == Opcode::STORE && func3 == 0b000) { instr.name = InstructionName::SB; return; }
	if (instr.opcode == Opcode::STORE && func3 == 0b001) { instr.name = InstructionName::SH; return; }
	if (instr.opcode == Opcode::STORE && func3 == 0b010) { instr.name = InstructionName::SW; return; }
	/*else*/ instr.name = InstructionName::Unknown;
}

void decodeBType(Instruction& instr){
	uint32_t imm_11		= (instr.raw & 0x00000080) >>  7;
	uint32_t imm_4_1	= (instr.raw & 0x00000F00) >>  8;
	uint32_t func3 		= (instr.raw & 0x00007000) >> 12;
	uint32_t source1 	= (instr.raw & 0x000F8000) >> 15;
	uint32_t source2 	= (instr.raw & 0x01F00000) >> 20;
	uint32_t imm_10_5 	= (instr.raw & 0x7E000000) >> 25;
	uint32_t imm_12 	= (instr.raw & 0x80000000) >> 31;

	instr.func3 		= func3;
	instr.source1 		= static_cast<Register>(source1);
	instr.source2 		= static_cast<Register>(source2);
	instr.immediate		= { (imm_12 << 12) + (imm_11 << 11) + (imm_10_5 << 5) + (imm_4_1 << 1), 0x00001FFE };


	if (instr.opcode == Opcode::BRANCH && func3 == 0b000) { instr.name = InstructionName::BEQ;	return; }
	if (instr.opcode == Opcode::BRANCH && func3 == 0b001) { instr.name = InstructionName::BNE; 	return; }
	if (instr.opcode == Opcode::BRANCH && func3 == 0b100) { instr.name = InstructionName::BLT; 	return; }
	if (instr.opcode == Opcode::BRANCH && func3 == 0b101) { instr.name = InstructionName::BGE; 	return; }
	if (instr.opcode == Opcode::BRANCH && func3 == 0b110) { instr.name = InstructionName::BLTU; return; }
	if (instr.opcode == Opcode::BRANCH && func3 == 0b111) { instr.name = InstructionName::BGEU; return; }
	/*else*/ instr.name = InstructionName::Unknown;
}

void decodeUType(Instruction& instr){
	uint32_t destination 	= (instr.raw & 0x00000F80) >>  7;
	uint32_t imm_31_12 		= (instr.raw & 0xFFFFF000) >> 12;

	instr.destination 		= static_cast<Register>(destination);
	instr.immediate			= { (imm_31_12 << 12), 0xFFFFF000 };

	if (instr.opcode == Opcode::LUI) 	{ instr.name = InstructionName::LUI; 	return;}
	if (instr.opcode == Opcode::AUIPC) 	{ instr.name = InstructionName::AUIPC; 	return;}
	/*else*/ instr.name = InstructionName::Unknown;
}

void decodeJType(Instruction& instr){
	uint32_t destination 	= (instr.raw & 0x00000F80) >>  7;
	uint32_t imm_19_12 		= (instr.raw & 0x000FF000) >> 12;
	uint32_t imm_11 		= (instr.raw & 0x00100000) >> 20;
	uint32_t imm_10_1 		= (instr.raw & 0x7FE00000) >> 21;
	uint32_t imm_20 		= (instr.raw & 0x80000000) >> 31;

	instr.name = InstructionName::JAL;
	instr.destination = static_cast<Register>(destination);
	instr.immediate	= { (imm_20 << 20) + (imm_19_12 << 12) + (imm_11 << 11) + (imm_10_1 << 1), 0x001FFFFE };
}

void dispatchDecode(Instruction& instr){
	switch (instr.type) {
	case InstructionType::R:
		decodeRType(instr);
		break;
	case InstructionType::I:
		decodeIType(instr);
		break;
	case InstructionType::S:
		decodeSType(instr);
		break;
	case InstructionType::B:
		decodeBType(instr);
		break;
	case InstructionType::U:
		decodeUType(instr);
		break;
	case InstructionType::J:
		decodeJType(instr);
		break;
	default:
		break;
	}
}

Instruction decode(RawInstruction raw) {
	Instruction instr = { raw };

	instr.opcode = static_cast<Opcode>(raw & 0x0000007F);
	getType(instr);
	dispatchDecode(instr);

	return instr;
}

std::ostream& operator<<(std::ostream& stream, Opcode& code){
	switch (code) {
	case Opcode::LUI: 		stream << "LUI"; 	break;
	case Opcode::AUIPC: 	stream << "AUIPC"; 	break;
	case Opcode::JAL: 		stream << "JAL"; 	break;
	case Opcode::JALR: 		stream << "JALR"; 	break;
	case Opcode::BRANCH: 	stream << "BRANCH"; break;
	case Opcode::LOAD: 		stream << "LOAD"; 	break;
	case Opcode::STORE: 	stream << "STORE"; 	break;
	case Opcode::ALUI: 		stream << "ALUI"; 	break;
	case Opcode::ALU: 		stream << "ALU"; 	break;
	case Opcode::FENCE: 	stream << "FENCE"; 	break;
	case Opcode::SYS: 		stream << "SYS"; 	break;
	default: stream << "Unknown opcode type"; 	break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Register& reg){
	uint32_t nb = static_cast<uint32_t>(reg);
	stream << 'x' << nb;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Immediate& imm){
	for (uint32_t mask = 0x80000000; mask; mask >>= 1) {
		stream <<
			(imm.immediate_mask & mask ?
				(imm.immediate & mask ? '1' : '0')
			:	'-');
	}
	stream << '(' << "0x" << std::hex << imm.immediate << "; " << std::dec << imm.immediate << ')';
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const InstructionType& type){
	stream << "Type : ";
	switch (type) {
	case InstructionType::R: stream << "R"; 	break;
	case InstructionType::I: stream << "I"; 	break;
	case InstructionType::S: stream << "S"; 	break;
	case InstructionType::B: stream << "B"; 	break;
	case InstructionType::U: stream << "U"; 	break;
	case InstructionType::J: stream << "J"; 	break;
	case InstructionType::Unknown: 	stream << "Unknown"; 	break;
	default: stream << "Unknown"; 	break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const InstructionName& name){
	stream << "Name : ";
	switch (name) {
	case InstructionName::LUI: 		stream << "LUI"; 	break;
 	case InstructionName::AUIPC: 	stream << "AUIPC"; 	break;
	case InstructionName::JAL: 		stream << "JAL"; 	break;
 	case InstructionName::JALR: 	stream << "JALR"; 	break;
	case InstructionName::BEQ: 		stream << "BEQ"; 	break;
 	case InstructionName::BNE: 		stream << "BNE"; 	break;
 	case InstructionName::BLT: 		stream << "BLT"; 	break;
 	case InstructionName::BGE: 		stream << "BGE"; 	break;
 	case InstructionName::BLTU: 	stream << "BLTU"; 	break;
 	case InstructionName::BGEU: 	stream << "BGEU"; 	break;
	case InstructionName::LB: 		stream << "LB"; 	break;
 	case InstructionName::LH: 		stream << "LH"; 	break;
 	case InstructionName::LW: 		stream << "LW"; 	break;
 	case InstructionName::LBU: 		stream << "LBU"; 	break;
 	case InstructionName::LHU: 		stream << "LHU"; 	break;
	case InstructionName::SB: 		stream << "SB"; 	break;
 	case InstructionName::SH: 		stream << "SH"; 	break;
 	case InstructionName::SW: 		stream << "SW"; 	break;
	case InstructionName::ADDI: 	stream << "ADDI"; 	break;
 	case InstructionName::SLTI: 	stream << "SLTI"; 	break;
 	case InstructionName::SLTIU: 	stream << "SLTIU"; 	break;
 	case InstructionName::XORI: 	stream << "XORI"; 	break;
 	case InstructionName::ORI: 		stream << "ORI"; 	break;
 	case InstructionName::ANDI: 	stream << "ANDI"; 	break;
 	case InstructionName::SLLI: 	stream << "SLLI"; 	break;
 	case InstructionName::SRLI: 	stream << "SRLI"; 	break;
 	case InstructionName::SRAI: 	stream << "SRAI"; 	break;
	case InstructionName::ADD: 		stream << "ADD"; 	break;
 	case InstructionName::SUB: 		stream << "SUB"; 	break;
 	case InstructionName::SLL: 		stream << "SLL"; 	break;
 	case InstructionName::SLT: 		stream << "SLT"; 	break;
 	case InstructionName::SLTU: 	stream << "SLTU"; 	break;
 	case InstructionName::XOR: 		stream << "XOR"; 	break;
 	case InstructionName::SRL: 		stream << "SRL"; 	break;
 	case InstructionName::SRA: 		stream << "SRA"; 	break;
 	case InstructionName::OR: 		stream << "OR"; 	break;
 	case InstructionName::AND: 		stream << "AND"; 	break;
	case InstructionName::FENCE: 	stream << "FENCE"; 	break;
	case InstructionName::ECALL: 	stream << "ECALL"; 	break;
 	case InstructionName::EBREAK: 	stream << "EBREAK"; break;
	case InstructionName::MUL: 		stream << "MUL"; 	break;
 	case InstructionName::MULH: 	stream << "MULH"; 	break;
 	case InstructionName::MULHSU: 	stream << "MULHSU"; break;
 	case InstructionName::MULHU: 	stream << "MULHU"; 	break;
 	case InstructionName::DIV: 		stream << "DIV"; 	break;
 	case InstructionName::DIVU: 	stream << "DIVU"; 	break;
 	case InstructionName::REM: 		stream << "REM"; 	break;
	case InstructionName::REMU: 	stream << "REMU"; 	break;
	default: stream << "Unkown(" << static_cast<uint32_t>(name) << ')'; break;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Instruction& instr){
	stream << "{ ";
	switch (instr.type) {
	case InstructionType::R:
		if (	instr.name == InstructionName::SLLI
			||	instr.name == InstructionName::SRLI
			||	instr.name == InstructionName::SRAI)
		{
			stream << instr.type << " ; "
				<< instr.name << " ; "
				<< "Destination register : " << instr.destination << " ; "
				<< "Source 1 register : " << instr.source1 << " ; "
				<< "Shift amount : " << instr.shift_amount;
		}
		else {
			stream << instr.type << " ; "
				<< instr.name << " ; "
				<< "Destination register : " << instr.destination << " ; "
				<< "Source 1 register : " << instr.source1 << " ; "
				<< "Source 2 register : " << instr.source2;
		}
		break;
	case InstructionType::I:
		stream << instr.type << " ; "
			<< instr.name << " ; "
			<< "Destination register : " << instr.destination << " ; "
			<< "Source 1 register : " << instr.source1 << " ; "
			<< "Immediate : " << instr.immediate;
		break;
	case InstructionType::S:
		stream << instr.type << " ; "
			<< instr.name << " ; "
			<< "Source 1 register : " << instr.source1 << " ; "
			<< "Source 2 register : " << instr.source2 << " ; "
			<< "Immediate : " << instr.immediate;
		break;
	case InstructionType::B:
		stream << instr.type << " ; "
			<< instr.name << " ; "
			<< "Source 1 register : " << instr.source1 << " ; "
			<< "Source 2 register : " << instr.source2 << " ; "
			<< "Immediate : " << instr.immediate;
		break;
	case InstructionType::U:
		stream << instr.type << " ; "
			<< instr.name << " ; "
			<< "Destination register : " << instr.destination << " ; "
			<< "Immediate : " << instr.immediate;
		break;
	case InstructionType::J:
		stream << instr.type << " ; "
			<< instr.name << " ; "
			<< "Destination register : " << instr.destination << " ; "
			<< "Immediate : " << instr.immediate;
		break;
	default:
		stream << instr.type << " ; "
			<< "Opcode : " << static_cast<uint32_t>(instr.opcode)
			<< "Raw : " <<  instr.raw;
		break;
	}
	stream << " }";
	// Special cases
	// if (instr.name == InstructionName::FENCE) {
	// 	/* code */
	// 	return stream;
	// }
	// if (	instr.name == InstructionName::EBREAK
	// 	||	instr.name == InstructionName::ECALL)
	// {
	// 	/* code */
	// 	return stream;
	// }

	return stream;
}