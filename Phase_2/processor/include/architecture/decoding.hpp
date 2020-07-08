#pragma once

#include "common.hpp"

// type definition for specific parts of an instruction
typedef ap_uint<7>  opcode_t;
typedef ap_uint<2>  opcode_prefix_t;
typedef ap_uint<5>  opcode_suffix_t;
typedef ap_uint<3>  opcode_suffix_low_t;
typedef ap_uint<2>  opcode_suffix_high_t;
typedef ap_uint<3>  func3_t;
typedef ap_uint<7>  func7_t;
typedef ap_uint<20> packed_immediate_t;

#define SCOPED_ENUM(type, ...) namespace type { enum type { __VA_ARGS__ }; }

// ┌─┬┐
// │┼├┤
// └─┴┘

/* Opcode structure
 *
 * The prefix and suffix of the opcode are defined with respect to
 * the bit position, which is from right to left
 *
 *  7    6 5   2 1        0
 * ┌────────────┬──────────┐
 * │   suffix   │          │
 * ├──────┬─────┤  prefix  │
 * │ high │ low │          │
 * └──────┴─────┴──────────┘
 *
 */
namespace Opcode {
	SCOPED_ENUM(Prefix,
		// only 11 is a valid prefix
	)

	namespace Suffix {
		enum {
			lui    = 0b01101,
			auipc  = 0b00101,
			jal    = 0b11011,
			jalr   = 0b11001,
			branch = 0b11000,
			load   = 0b00000,
			store  = 0b01000,
			alui   = 0b00100,
			alu    = 0b01100,
			fence  = 0b00011,
			system = 0b11100
		};

		SCOPED_ENUM(High,
			load_fence_auipc    = 0b00,
			store_alu_lui       = 0b01,
			branch_jal_r_system = 0b11
		)

		SCOPED_ENUM(Low,
			// hXX represents the two bits of the high suffix associated with the low opcode
			h00_load  = 0b000,
			h00_fence = 0b011,
			h00_auipc = 0b101,

			h01_store = 0b000,
			h01_alu   = 0b100,
			h01_lui   = 0b101,

			h11_branch = 0b000,
			h11_jalr   = 0b001,
			h11_jal    = 0b011,
			h11_system = 0b100,
		)
	}
}

SCOPED_ENUM(Func3,
	addi      = 0b000,
	slli      = 0b001,
	slti      = 0b010,
	sltiu     = 0b011,
	xori      = 0b100,
	srli_srai = 0b101,
	ori       = 0b110,
	andi      = 0b111,

	add_sub_mul  = 0b000,
	sll_mulh     = 0b001,
	slt_mulhsu   = 0b010,
	sltu_mulhu   = 0b011,
	xor_div      = 0b100,
	srl_sra_divu = 0b101,
	or_rem       = 0b110,
	and_remu     = 0b111,

	jalr = 0b000,

	lb  = 0b000,
	lh  = 0b001,
	lw  = 0b010,
	lbu = 0b100,
	lhu = 0b101,

	fence = 0b000,

	sb  = 0b000,
	sh  = 0b001,
	sw  = 0b010,

	beq  = 0b000,
	bne  = 0b001,
	blt  = 0b100,
	bge  = 0b101,
	bltu = 0b110,
	bgeu = 0b111,
)

SCOPED_ENUM(Func7,
	null  = 0b0000000,
	first = 0b0000001,
	sixth = 0b0100000,
)

/* Symbolic constants for decoding
 *
 */