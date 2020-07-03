#pragma once

#include <string>

#include "../include/common.hpp"

typedef ap_uint<7> opcode_t;
typedef ap_uint<2> opcode_prefix_t;
typedef ap_uint<5> opcode_suffix_t;
typedef ap_uint<3> opcode_suffix_low_t;
typedef ap_uint<2> opcode_suffix_high_t;
typedef ap_uint<3> func3_t;
typedef ap_uint<7> func7_t;
typedef ap_uint<20> packed_immediate_t;

namespace Slicing {
	namespace Opcode {
		const uint8_t low            = 0;
		const uint8_t high           = 6;
	}

	namespace OpcodePrefix {
		const uint8_t low            = 0;
		const uint8_t high           = 1;
	}

	namespace OpcodeSuffix {
		const uint8_t low            = 2;
		const uint8_t high           = 6;
	}

	namespace OpcodeSuffixLow {
		const uint8_t low            = 2;
		const uint8_t high           = 4;
	}

	namespace OpcodeSuffixHigh {
		const uint8_t low            = 5;
		const uint8_t high           = 6;
	}

	namespace Func3 {
		const uint8_t low            = 12;
		const uint8_t high           = 14;
	}

	namespace Func7 {
		const uint8_t low            = 25;
		const uint8_t high           = 31;
	}

	namespace Dest {
		const uint8_t low            = 7;
		const uint8_t high           = 11;
	}

	namespace Reg1 {
		const uint8_t low            = 15;
		const uint8_t high           = 19;
	}

	namespace Reg2 {
		const uint8_t low            = 20;
		const uint8_t high           = 24;
	}

	namespace R {

	}

	namespace I {
		const uint8_t imm_11_0_low   = 20;
		const uint8_t imm_11_0_high  = 31;

		const uint8_t imm_sign       = 31;
	}

	namespace S {
		const uint8_t imm_4_0_low    = 7;
		const uint8_t imm_4_0_high   = 11;

		const uint8_t imm_11_5_low   = 25;
		const uint8_t imm_11_5_high  = 31;

		const uint8_t imm_sign       = 31;
	}

	namespace B {
		const uint8_t imm_4_1_low    = 8;
		const uint8_t imm_4_1_high   = 11;

		const uint8_t imm_10_5_low   = 25;
		const uint8_t imm_10_5_high  = 30;

		const uint8_t imm_11         = 7;

		const uint8_t imm_12         = 31;

		const uint8_t imm_sign       = 31;
	}

	namespace U {
		const uint8_t imm_31_12_low  = 12;
		const uint8_t imm_31_12_high = 31;
	}

	namespace J {
		const uint8_t imm_10_1_low   = 21;
		const uint8_t imm_10_1_high  = 30;

		const uint8_t imm_19_12_low  = 12;
		const uint8_t imm_19_12_high = 19;

		const uint8_t imm_11         = 20;

		const uint8_t imm_20         = 31;

		const uint8_t imm_sign       = 31;
	}
}

namespace Instruction {
	enum Type {
		R, I, S, B, U, J, UnknownType
	};

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
}

namespace Opcode32Suffix {
	const opcode_suffix_t lui    = 0b01101;
	const opcode_suffix_t auipc  = 0b00101;
	const opcode_suffix_t jal    = 0b11011;
	const opcode_suffix_t jalr   = 0b11001;
	const opcode_suffix_t branch = 0b11000;
	const opcode_suffix_t load   = 0b00000;
	const opcode_suffix_t store  = 0b01000;
	const opcode_suffix_t alui   = 0b00100;
	const opcode_suffix_t alu    = 0b01100;
	const opcode_suffix_t fence  = 0b00011;
	const opcode_suffix_t system = 0b11100;

	#define OPCODE_SUFFIX_LUI    0b01101
	#define OPCODE_SUFFIX_AUIPC  0b00101
	#define OPCODE_SUFFIX_JAL    0b11011
	#define OPCODE_SUFFIX_JALR   0b11001
	#define OPCODE_SUFFIX_BRANCH 0b11000
	#define OPCODE_SUFFIX_LOAD   0b00000
	#define OPCODE_SUFFIX_STORE  0b01000
	#define OPCODE_SUFFIX_ALUI   0b00100
	#define OPCODE_SUFFIX_ALU    0b01100
	#define OPCODE_SUFFIX_FENCE  0b00011
	#define OPCODE_SUFFIX_SYSTEM 0b11100
}

namespace OpcodeHigh {
	const opcode_suffix_high_t load_fence_auipc    = 0b00;
	const opcode_suffix_high_t store_alu_lui       = 0b01;
	const opcode_suffix_high_t branch_jal_r_system = 0b11;
	#define OPCODE_HIGH_00_LOAD_FENCE_ALUI_AUIPC  0b00
	#define OPCODE_HIGH_01_STORE_ALU_LUI          0b01
	#define OPCODE_HIGH_10                        0b10
	#define OPCODE_HIGH_11_BRANCH_JAL_R_SYSTEM    0b11
}

namespace OpcodeLow {
	// High is 0b00
	const opcode_suffix_low_t load  = 0b000;
	const opcode_suffix_low_t fence = 0b011;
	const opcode_suffix_low_t auipc = 0b101;
	#define OPCODE_LOW_00_LOAD  0b000
	#define OPCODE_LOW_00_001   0b001
	#define OPCODE_LOW_00_010   0b010
	#define OPCODE_LOW_00_FENCE 0b011
	#define OPCODE_LOW_00_ALUI  0b100
	#define OPCODE_LOW_00_AUIPC 0b101
	#define OPCODE_LOW_00_110   0b110
	#define OPCODE_LOW_00_111   0b111

	// High is 0b01
	const opcode_suffix_low_t store = 0b000;
	const opcode_suffix_low_t alu   = 0b100;
	const opcode_suffix_low_t lui   = 0b101;
	#define OPCODE_LOW_01_STORE 0b000
	#define OPCODE_LOW_01_001   0b001
	#define OPCODE_LOW_01_010   0b010
	#define OPCODE_LOW_01_011   0b011
	#define OPCODE_LOW_01_ALU   0b100
	#define OPCODE_LOW_01_LUI   0b101
	#define OPCODE_LOW_01_110   0b110
	#define OPCODE_LOW_01_111   0b111

	// High is 0b11
	const opcode_suffix_low_t branch = 0b000;
	const opcode_suffix_low_t jalr   = 0b001;
	const opcode_suffix_low_t jal    = 0b011;
	const opcode_suffix_low_t system = 0b100;
	#define OPCODE_LOW_11_BRANCH 0b000
	#define OPCODE_LOW_11_JALR   0b001
	#define OPCODE_LOW_11_010    0b010
	#define OPCODE_LOW_11_JAL    0b011
	#define OPCODE_LOW_11_SYSTEM 0b100
	#define OPCODE_LOW_11_101    0b101
	#define OPCODE_LOW_11_110    0b110
	#define OPCODE_LOW_11_111    0b111
}

namespace Func3 {
	const func3_t addi      = 0b000;
	const func3_t slli      = 0b001;
	const func3_t slti      = 0b010;
	const func3_t sltiu     = 0b011;
	const func3_t xori      = 0b100;
	const func3_t srli_srai = 0b101;
	const func3_t ori       = 0b110;
	const func3_t andi      = 0b111;

	#define FUNC3_ADDI      0b000
	#define FUNC3_SLLI      0b001
	#define FUNC3_SLTI      0b010
	#define FUNC3_SLTIU     0b011
	#define FUNC3_XORI      0b100
	#define FUNC3_SRLI_SRAI 0b101
	#define FUNC3_ORI       0b110
	#define FUNC3_ANDI      0b111

	const func3_t add_sub_mul  = 0b000;
	const func3_t sll_mulh     = 0b001;
	const func3_t slt_mulhsu   = 0b010;
	const func3_t sltu_mulhu   = 0b011;
	const func3_t xor_div      = 0b100;
	const func3_t srl_sra_divu = 0b101;
	const func3_t or_rem       = 0b110;
	const func3_t and_remu     = 0b111;

	#define FUNC3_ADD_SUB_MUL  0b000
	#define FUNC3_SLL_MULH     0b001
	#define FUNC3_SLT_MULHSU   0b010
	#define FUNC3_SLTU_MULHU   0b011
	#define FUNC3_XOR_DIV      0b100
	#define FUNC3_SRL_SRA_DIVU 0b101
	#define FUNC3_OR_REM       0b110
	#define FUNC3_AND_REMU     0b111

	const func3_t jalr = 0b000;

	#define FUNC3_JALR 0b000

	const func3_t lb  = 0b000;
	const func3_t lh  = 0b001;
	const func3_t lw  = 0b010;
	const func3_t lbu = 0b100;
	const func3_t lhu = 0b101;

	#define FUNC3_LB   0b000
	#define FUNC3_LH   0b001
	#define FUNC3_LW   0b010
	#define FUNC3_L011 0b011
	#define FUNC3_LBU  0b100
	#define FUNC3_LHU  0b101
	#define FUNC3_L110 0b110
	#define FUNC3_L111 0b111

	const func3_t fence = 0b000;

	#define FUNC3_FENCE 0b000

	const func3_t sb  = 0b000;
	const func3_t sh  = 0b001;
	const func3_t sw  = 0b010;

	#define FUNC3_SB 0b000
	#define FUNC3_SH 0b001
	#define FUNC3_SW 0b010

	const func3_t beq  = 0b000;
	const func3_t bne  = 0b001;
	const func3_t blt  = 0b100;
	const func3_t bge  = 0b101;
	const func3_t bltu = 0b110;
	const func3_t bgeu = 0b111;

	#define FUNC3_BEQ  0b000
	#define FUNC3_BNE  0b001
	#define FUNC3_B010 0b010
	#define FUNC3_B011 0b011
	#define FUNC3_BLT  0b100
	#define FUNC3_BGE  0b101
	#define FUNC3_BLTU 0b110
	#define FUNC3_BGEU 0b111
}

namespace Func7 {
	const func7_t null  = 0b0000000;
	const func7_t first = 0b0000001;
	const func7_t sixth = 0b0100000;

	#define FUNC7_NULL  0b0000000
	#define FUNC7_FIRST 0b0000001
	#define FUNC7_SIXTH 0b0100000
}

namespace Register {
	const reg_t  x0 = 0b00000, zero = 0b00000;
	const reg_t  x1 = 0b00001,   ra = 0b00001;
	const reg_t  x2 = 0b00010,   sp = 0b00010;
	const reg_t  x3 = 0b00011,   gp = 0b00011;
	const reg_t  x4 = 0b00100,   tp = 0b00100;
	const reg_t  x5 = 0b00101,   t0 = 0b00101;
	const reg_t  x6 = 0b00110,   t1 = 0b00110;
	const reg_t  x7 = 0b00111,   t2 = 0b00111;
	const reg_t  x8 = 0b01000,   s0 = 0b01000;
	const reg_t  x9 = 0b01001,   s1 = 0b01001;
	const reg_t x10 = 0b01010,   a0 = 0b01010;
	const reg_t x11 = 0b01011,   a1 = 0b01011;
	const reg_t x12 = 0b01100,   a2 = 0b01100;
	const reg_t x13 = 0b01101,   a3 = 0b01101;
	const reg_t x14 = 0b01110,   a4 = 0b01110;
	const reg_t x15 = 0b01111,   a5 = 0b01111;
	const reg_t x16 = 0b10000,   a6 = 0b10000;
	const reg_t x17 = 0b10001,   a7 = 0b10001;
	const reg_t x18 = 0b10010,   s2 = 0b10010;
	const reg_t x19 = 0b10011,   s3 = 0b10011;
	const reg_t x20 = 0b10100,   s4 = 0b10100;
	const reg_t x21 = 0b10101,   s5 = 0b10101;
	const reg_t x22 = 0b10110,   s6 = 0b10110;
	const reg_t x23 = 0b10111,   s7 = 0b10111;
	const reg_t x24 = 0b11000,   s8 = 0b11000;
	const reg_t x25 = 0b11001,   s9 = 0b11001;
	const reg_t x26 = 0b11010,  s10 = 0b11010;
	const reg_t x27 = 0b11011,  s11 = 0b11011;
	const reg_t x28 = 0b11100,   t3 = 0b11100;
	const reg_t x29 = 0b11101,   t4 = 0b11101;
	const reg_t x30 = 0b11110,   t5 = 0b11110;
	const reg_t x31 = 0b11111,   t6 = 0b11111;

	#define REG_X0   0b00000
	#define REG_ZERO 0b00000
	#define REG_X1   0b00001
	#define REG_RA   0b00001
	#define REG_X2   0b00010
	#define REG_SP   0b00010
	#define REG_X3   0b00011
	#define REG_GP   0b00011
	#define REG_X4   0b00100
	#define REG_TP   0b00100
	#define REG_X5   0b00101
	#define REG_T0   0b00101
	#define REG_X6   0b00110
	#define REG_T1   0b00110
	#define REG_X7   0b00111
	#define REG_T2   0b00111
	#define REG_X8   0b01000
	#define REG_S0   0b01000
	#define REG_X9   0b01001
	#define REG_S1   0b01001
	#define REG_X10  0b01010
	#define REG_A0   0b01010
	#define REG_X11  0b01011
	#define REG_A1   0b01011
	#define REG_X12  0b01100
	#define REG_A2   0b01100
	#define REG_X13  0b01101
	#define REG_A3   0b01101
	#define REG_X14  0b01110
	#define REG_A4   0b01110
	#define REG_X15  0b01111
	#define REG_A5   0b01111
	#define REG_X16  0b10000
	#define REG_A6   0b10000
	#define REG_X17  0b10001
	#define REG_A7   0b10001
	#define REG_X18  0b10010
	#define REG_S2   0b10010
	#define REG_X19  0b10011
	#define REG_S3   0b10011
	#define REG_X20  0b10100
	#define REG_S4   0b10100
	#define REG_X21  0b10101
	#define REG_S5   0b10101
	#define REG_X22  0b10110
	#define REG_S6   0b10110
	#define REG_X23  0b10111
	#define REG_S7   0b10111
	#define REG_X24  0b11000
	#define REG_S8   0b11000
	#define REG_X25  0b11001
	#define REG_S9   0b11001
	#define REG_X26  0b11010
	#define REG_S10  0b11010
	#define REG_X27  0b11011
	#define REG_S11  0b11011
	#define REG_X28  0b11100
	#define REG_T3   0b11100
	#define REG_X29  0b11101
	#define REG_T4   0b11101
	#define REG_X30  0b11110
	#define REG_T5   0b11110
	#define REG_X31  0b11111
	#define REG_T6   0b11111

}

std::string to_string(Instruction::Type type);
std::string to_string(Instruction::Name name);
std::string to_string(const reg_t& reg);