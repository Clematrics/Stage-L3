#pragma once

#include "../include/common.hpp"
#include "../include/instructions.hpp"

#define ITERATION4

#ifdef ITERATION1
enum InstructionType {
	R, I, S, B, U, J, Unknwon
};

struct DecodedInstruction {
	ap_uint<6> opcode;
	InstructionType type;
	ap_uint<3> func3;
	ap_uint<7> func7;
	ap_uint<5> dest;
	ap_uint<5> reg1;
	ap_uint<5> reg2;
	word_t immediate;
	bit_t invalid;
};
#endif

// #ifdef ITERATION2
// namespace Instruction {
// 	enum Name {
// 		LUI, AUIPC,
// 		JAL, JALR,
// 		BEQ, BNE, BLT, BGE, BLTU, BGEU,
// 		LB, LH, LW, LBU, LHU,
// 		SB, SH, SW,
// 		ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI,
// 		ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
// 		FENCE,
// 		ECALL, EBREAK,
// 		MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,
// 		Unknown
// 	};
// }

// struct DecodedInstruction {
// 	Instruction::Name name;
// 	ap_uint<5> dest;
// 	ap_uint<5> reg1;
// 	ap_uint<5> reg2;
// 	word_t immediate;
// };
// #endif

// #ifdef ITERATION3
// namespace Instruction {
// 	enum Name {
// 		LUI, AUIPC,
// 		JAL, JALR,
// 		BEQ, BNE, BLT, BGE, BLTU, BGEU,
// 		LB, LH, LW, LBU, LHU,
// 		SB, SH, SW,
// 		ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI,
// 		ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
// 		FENCE,
// 		ECALL, EBREAK,
// 		MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,
// 		Unknown
// 	};
// }

// struct DecodedInstruction {
// 	Instruction::Name name;
// 	reg_t dest;
// 	reg_t reg1;
// 	reg_t reg2;
// 	word_t immediate;
// };
// #endif

#if defined(ITERATION2) || defined(ITERATION3) || defined(ITERATION4) || defined(ITERATION5)
#include <string>

namespace Instruction {
	enum Type {
		R, I, S, B, U, J, UnknownType
	};

	std::string getType(Type type);

	enum Name {
		LUI, AUIPC,
		JAL, JALR,
		BEQ, BNE, BLT, BGE, BLTU, BGEU,
		LB, LH, LW, LBU, LHU,
		SB, SH, SW,
		ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI,
		ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
		FENCE,
		ECALL, EBREAK,
		MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,
		UnknownName
	};

	std::string getName(Name name);
}


struct DecodedInstruction {
	Instruction::Name name;
	reg_t dest;
	reg_t reg1;
	reg_t reg2;
	word_t immediate;
};
#endif

class Decode {
	#ifdef OTHER
	InstructionType instructionType(const word_t instruction);
	#endif
	bool active;
public:
	Decode();
	void decode(const word_t instruction, word_t program_counter, word_t* out_program_counter, bit_t* stop_signal, DecodedInstruction* decoded);
};
