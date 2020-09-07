#pragma once

#include "common.hpp"

#define SCOPED_ENUM(type, ...) namespace type { enum type { __VA_ARGS__ }; }

namespace Architecture {
	SCOPED_ENUM(Register,
		x0  = 0b00000, x1  = 0b00001, x2  = 0b00010, x3  = 0b00011, x4  = 0b00100, x5  = 0b00101, x6  = 0b00110, x7  = 0b00111,
		x8  = 0b01000, x9  = 0b01001, x10 = 0b01010, x11 = 0b01011, x12 = 0b01100, x13 = 0b01101, x14 = 0b01110, x15 = 0b01111,
		x16 = 0b10000, x17 = 0b10001, x18 = 0b10010, x19 = 0b10011, x20 = 0b10100, x21 = 0b10101, x22 = 0b10110, x23 = 0b10111,
		x24 = 0b11000, x25 = 0b11001, x26 = 0b11010, x27 = 0b11011, x28 = 0b11100, x29 = 0b11101, x30 = 0b11110, x31 = 0b11111,

		zero = x0,  ra = x1,  sp  = x2,  gp  = x3,  tp = x4,  t0 = x5,  t1 = x6,  t2 = x7,
		s0   = x8,  s1 = x9,  a0  = x10, a1  = x11, a2 = x12, a3 = x13, a4 = x14, a5 = x15,
		a6   = x16, a7 = x17, s2  = x18, s3  = x19, s4 = x20, s5 = x21, s6 = x22, s7 = x23,
		s8   = x24, s9 = x25, s10 = x26, s11 = x27, t3 = x28, t4 = x29, t5 = x30, t6 = x31
	)

	SCOPED_ENUM(Type,
		R, I, S, B, U, J, unknown_type
	)

	SCOPED_ENUM(Name,
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
		unknown_name
	)

	SCOPED_ENUM(InstructionCategory,
		load, fence, alui, auipc, store, alu, lui, branch, jalr, jal, system,
		unknown_kind
	)
}