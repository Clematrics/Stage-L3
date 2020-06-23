#pragma once

#include "common.hpp"

typedef ap_uint<7> opcode_t;
typedef ap_uint<2> opcode_prefix_t;
typedef ap_uint<5> opcode_suffix_t;
typedef ap_uint<3> func3_t;
typedef ap_uint<7> func7_t;
typedef ap_uint<5> reg_t;

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

	const func3_t sb  = 0b000;
	const func3_t sh  = 0b001;
	const func3_t sw  = 0b010;

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
}