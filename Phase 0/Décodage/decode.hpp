#include <cstdint>
#include <iostream>

using RawInstruction = uint32_t;

enum class InstructionType {
	R, I, S, B, U, J, Unknown
};

enum class Register {
	x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15,
	x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31
};

enum class Opcode {
	LUI		= 0b0110111,
	AUIPC	= 0b0010111,
	JAL		= 0b1101111,
	JALR	= 0b1100111,
	BRANCH	= 0b1100011,	// BEQ, BNE, BLT, BGE, BLTU, BGEU
	LOAD	= 0b0000011,	// LB, LH, LW, LBU, LHU
	STORE	= 0b0100011,	// SB, SH, SW
	ALUI	= 0b0010011,	// ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI
	ALU		= 0b0110011,	// ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND
							// from M extension : MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU
	FENCE	= 0b0001111,
	SYS		= 0b1110011,	// ECALL, EBREAK
};

enum class InstructionName {
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
	Unknown
};

struct Immediate {
	uint32_t immediate;
	uint32_t immediate_mask;
};

struct Instruction {
	RawInstruction raw;
	InstructionName name;
	InstructionType type;
	Opcode opcode;
	uint8_t func3;
	uint8_t func7;
	Register destination;
	Register source1;
	Register source2;
	Immediate immediate;
	uint8_t shift_amount;
};

// std::ostream& operator<<(std::ostream& stream, const Opcode& code);
// std::ostream& operator<<(std::ostream& stream, const Register& reg);
// std::ostream& operator<<(std::ostream& stream, const Immediate& imm);
// std::ostream& operator<<(std::ostream& stream, const InstructionType& type);
// std::ostream& operator<<(std::ostream& stream, const InstructionName& name);
std::ostream& operator<<(std::ostream& stream, const Instruction& info);

// void decodeRType(Instruction& info);
// void decodeIType(Instruction& info);
// void decodeSType(Instruction& info);
// void decodeBType(Instruction& info);
// void decodeUType(Instruction& info);
// void decodeJType(Instruction& info);
// void dispatchDecode(Instruction& info);
// void getType(Instruction& info);
Instruction decode(RawInstruction instr);