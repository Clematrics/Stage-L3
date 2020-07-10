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

/* Symbolic constants for the decoding phase */
namespace Decoding {
	// TODO : add categories ? jumps ?
	SCOPED_ENUM(Kind,
		alu,
		alu_immediate,
		load,
		store,
		branch,
		other,
		unknown_kind
	)

	/* The following is always extracted, regardless of the instruction type
	*
	*  31   25 24  20 19  15 14   12 11   7 6    5 4   2 1        0
	* ┌───────┬──────┬──────┬───────┬──────┬────────────┬──────────┐
	* │       │      │      │       │      │   suffix   │          │
	* │ func7 │ src2 │ src1 │ func3 │ dest │──────┬─────┤  prefix  │
	* │       │      │      │       │      │ high │ low │          │
	* └───────┴──────┴──────┴───────┴──────┴──────┴─────┴──────────┘
	*/
	SCOPED_ENUM(Func7,
		high = 31,
		low  = 25,

		null  = 0b0000000,
		first = 0b0000001,
		sixth = 0b0100000,
	)

	SCOPED_ENUM(Src2,
		high = 24,
		low  = 20
	)

	SCOPED_ENUM(Src1,
		high = 19,
		low  = 15
	)

	SCOPED_ENUM(Func3,
		high = 14,
		low  = 12,

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

	SCOPED_ENUM(Dest,
		high = 11,
		low  = 7
	)

	namespace Opcode {
		enum {
			high = 6,
			low  = 0,
		};

		namespace Suffix {
			enum {
				high = 6,
				low  = 2,
			};

			// Only for the disassembler
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
				high = 6,
				low  = 5,

				load_fence_auipc    = 0b00,
				store_alu_lui       = 0b01,
				h10_unused          = 0b10,
				branch_jal_r_system = 0b11
			)

			SCOPED_ENUM(Low,
				high = 4,
				low  = 2,

				// hXX represents the two bits of the high suffix associated with the low opcode
				h00_load  = 0b000,
				h00_001   = 0b001,
				h00_010   = 0b010,
				h00_fence = 0b011,
				h00_alui  = 0b100,
				h00_auipc = 0b101,
				h00_110   = 0b110,
				h00_111   = 0b111,

				h01_store = 0b000,
				h01_001   = 0b001,
				h01_010   = 0b010,
				h01_011   = 0b011,
				h01_alu   = 0b100,
				h01_lui   = 0b101,
				h01_110   = 0b110,
				h01_111   = 0b111,

				h11_branch = 0b000,
				h11_jalr   = 0b001,
				h11_010    = 0b010,
				h11_jal    = 0b011,
				h11_system = 0b100,
				h11_101    = 0b101,
				h11_110    = 0b110,
				h11_111    = 0b111
			)
		}

		SCOPED_ENUM(Prefix,
			high = 1,
			low  = 0
		)
	}

	/* According to the type, the immediate information is extracted and is mapped to a packed_immediate */

	/*       instruction immediate ─┐                                    packed immediate ─┐                                   unpacked immediate ────────────┐
	*         31       20 19  0                                           19 12 11        0                                     31              12 11        0
	*        ┌───────────┬─────┐                                         ┌─────┬───────────┐                                   ┌──────────────────┬───────────┐
	* I-type │ imm[11:0] │ *** │ --------------------------------------> │ *** │ imm[11:0] │ --------------------------------> │ imm[11] ? -1 : 0 │ imm[11:0] │
	*        └───────────┴─────┘                                         └─────┴───────────┘                                   └──────────────────┴───────────┘
	*/
	namespace I {
		namespace InInstruction {
			SCOPED_ENUM(Sign,
				pos  = 31
			)
			SCOPED_ENUM(Imm_11_0,
				high = 31,
				low  = 20
			)
		}
		namespace InPackedImmediate {
			SCOPED_ENUM(Sign,
				pos  = 11
			)
			SCOPED_ENUM(Imm_11_0,
				high = 11,
				low  = 0
			)
		}
		namespace InUnpackedImmediate {
			SCOPED_ENUM(SignExtension,
				high = 31,
				low  = 12,
			)
			SCOPED_ENUM(Imm_11_0,
				high = 11,
				low  = 0,
			)
		}
	}

	/*       instruction immediate ─────────────┐                        packed immediate ────────────┐                        unpacked immediate ───────────────────────┐
	*         31       25 24 12 11       7 6   0                          19 12 11        5 4        0                          31              12 11        5 4        0
	*        ┌───────────┬─────┬──────────┬─────┐                        ┌─────┬───────────┬──────────┐                        ┌──────────────────┬───────────┬──────────┐
	* S-type │ imm[11:5] │ *** │ imm[4:0] │ *** │ ---------------------> │ *** │ imm[11:5] │ imm[4:0] │ ---------------------> │ imm[11] ? -1 : 0 │ imm[11:5] │ imm[4:0] │
	*        └───────────┴─────┴──────────┴─────┘                        └─────┴───────────┴──────────┘                        └──────────────────┴───────────┴──────────┘
	*/
	namespace S {
		namespace InInstruction {
			SCOPED_ENUM(Sign,
				pos  = 31
			)
			SCOPED_ENUM(Imm_11_5,
				high = 31,
				low  = 25
			)
			SCOPED_ENUM(Imm_4_0,
				high = 11,
				low  = 7
			)
		}
		namespace InPackedImmediate {
			SCOPED_ENUM(Sign,
				pos  = 11
			)
			SCOPED_ENUM(Imm_11_5,
				high = 11,
				low  = 5
			)
			SCOPED_ENUM(Imm_4_0,
				high = 4,
				low  = 0
			)
			SCOPED_ENUM(Imm_11_0,
				high = 11,
				low  = 0
			)
		}
		namespace InUnpackedImmediate {
			SCOPED_ENUM(SignExtension,
				high = 31,
				low  = 12,
			)
			SCOPED_ENUM(Imm_11_5,
				high = 11,
				low  = 5,
			)
			SCOPED_ENUM(Imm_4_0,
				high = 4,
				low  = 0,
			)
			SCOPED_ENUM(Imm_11_0,
				high = 11,
				low  = 0,
			)
		}
	}

	/*       instruction immediate ─────────────────────────────────┐    packed immediate ────────────────────────────────┐    unpacked immediate ───────────────────────────────────────────────┐
	*             31    30       25 24 12 11       8     7     6   0      19 12     11        10    9         4 3        0      31              13     12       11     10        5 4        1  0
	*        ┌─────────┬───────────┬─────┬──────────┬─────────┬─────┐    ┌─────┬─────────┬─────────┬───────────┬──────────┐    ┌──────────────────┬─────────┬─────────┬───────────┬──────────┬───┐
	* B-type │ imm[12] │ imm[10:5] │ *** │ imm[4:1] │ imm[11] │ *** │ -> │ *** │ imm[12] │ imm[11] │ imm[10:5] │ imm[4:1] │ -> │ imm[12] ? -1 : 0 │ imm[12] │ imm[11] │ imm[10:5] │ imm[4:1] │ 0 │
	*        └─────────┴───────────┴─────┴──────────┴─────────┴─────┘    └─────┴─────────┴─────────┴───────────┴──────────┘    └──────────────────┴─────────┴─────────┴───────────┴──────────┴───┘
	*/
	namespace B {
		namespace InInstruction {
			SCOPED_ENUM(Sign,
				pos  = 31
			)
			SCOPED_ENUM(Imm_12,
				pos  = 31
			)
			SCOPED_ENUM(Imm_11,
				pos  = 7
			)
			SCOPED_ENUM(Imm_10_5,
				high = 30,
				low  = 25
			)
			SCOPED_ENUM(Imm_4_1,
				high = 11,
				low  = 8
			)
		}
		namespace InPackedImmediate {
			SCOPED_ENUM(Sign,
				pos  = 11
			)
			SCOPED_ENUM(Imm_12,
				pos  = 11
			)
			SCOPED_ENUM(Imm_11,
				pos  = 10
			)
			SCOPED_ENUM(Imm_10_5,
				high = 9,
				low  = 4
			)
			SCOPED_ENUM(Imm_4_1,
				high = 3,
				low  = 0
			)
			SCOPED_ENUM(Imm_12_1,
				high = 11,
				low  = 0
			)
		}
		namespace InUnpackedImmediate {
			SCOPED_ENUM(SignExtension,
				high = 31,
				low  = 13,
			)
			SCOPED_ENUM(Imm_12,
				pos  = 12
			)
			SCOPED_ENUM(Imm_11,
				pos  = 11
			)
			SCOPED_ENUM(Imm_10_5,
				high = 10,
				low  = 5
			)
			SCOPED_ENUM(Imm_4_1,
				high = 4,
				low  = 1
			)
			SCOPED_ENUM(Imm_0,
				pos   = 0,
				value = 0
			)
			SCOPED_ENUM(Imm_12_1,
				high  = 12,
				low   = 1
			)
		}
	}

	/*       instruction immediate ─┐                                    packed immediate ─┐                                   unpacked immediate ─┐
	*         31        12 11  0                                          19         0                                          31        12 11  0
	*        ┌────────────┬─────┐                                        ┌────────────┐                                        ┌────────────┬─────┐
	* U-type │ imm[31:12] │ *** │ -------------------------------------> │ imm[31:12] │ -------------------------------------> │ imm[31:12] │  0  │
	*        └────────────┴─────┘                                        └────────────┘                                        └────────────┴─────┘
	*/
	namespace U {
		namespace InInstruction {
			SCOPED_ENUM(Imm_31_12,
				high = 31,
				low  = 12
			)
		}
		namespace InPackedImmediate {
			SCOPED_ENUM(Imm_31_12,
				high = 19,
				low  = 0
			)
		}
		namespace InUnpackedImmediate {
			SCOPED_ENUM(Imm_31_12,
				high = 31,
				low  = 12
			)
			SCOPED_ENUM(Imm_11_0,
				high  = 11,
				low   = 0,
				value = 0x000
			)
		}
	}

	/*       instruction immediate ─────────────────────────────┐        packed immediate ────────────────────────────┐        unpacked immediate ─────────────────────────────────────────────────┐
	*             31    30       21     20    19        12 11  0              19    18        11     10    9         0          31              21     20    19        12    11     10        1  0
	*        ┌─────────┬───────────┬─────────┬────────────┬─────┐        ┌─────────┬────────────┬─────────┬───────────┐        ┌──────────────────┬─────────┬────────────┬─────────┬───────────┬───┐
	* J-type │ imm[20] │ imm[10:1] │ imm[11] │ imm[19:12] │ *** │ -----> │ imm[20] │ imm[19:12] │ imm[11] │ imm[10:1] │ -----> │ imm[20] ? -1 : 0 │ imm[20] │ imm[19:12] │ imm[11] │ imm[10:1] │ 0 │
	*        └─────────┴───────────┴─────────┴────────────┴─────┘        └─────────┴────────────┴─────────┴───────────┘        └──────────────────┴─────────┴────────────┴─────────┴───────────┴───┘
	*/
	namespace J {
		namespace InInstruction {
			SCOPED_ENUM(Sign,
				pos  = 31
			)
			SCOPED_ENUM(Imm_20,
				pos  = 31
			)
			SCOPED_ENUM(Imm_19_12,
				high = 19,
				low  = 12
			)
			SCOPED_ENUM(Imm_11,
				pos  = 20
			)
			SCOPED_ENUM(Imm_10_1,
				high = 30,
				low  = 21
			)
		}
		namespace InPackedImmediate {
			SCOPED_ENUM(Sign,
				pos  = 19
			)
			SCOPED_ENUM(Imm_20,
				pos  = 19
			)
			SCOPED_ENUM(Imm_19_12,
				high = 18,
				low  = 11
			)
			SCOPED_ENUM(Imm_11,
				pos  = 10
			)
			SCOPED_ENUM(Imm_10_1,
				high = 9,
				low  = 0
			)
			SCOPED_ENUM(Imm_20_1,
				high = 19,
				low  = 0
			)
			SCOPED_ENUM(JumpOffset,
				high = 19,
				low  = 1
			)
		}
		namespace InUnpackedImmediate {
			SCOPED_ENUM(SignExtension,
				high = 31,
				low  = 21
			)
			SCOPED_ENUM(Imm_20,
				pos  = 20
			)
			SCOPED_ENUM(Imm_19_12,
				high = 19,
				low  = 12
			)
			SCOPED_ENUM(Imm_11,
				pos  = 11
			)
			SCOPED_ENUM(Imm_10_1,
				high = 10,
				low  = 1
			)
			SCOPED_ENUM(Imm_0,
				pos   = 0,
				value = 0
			)
			SCOPED_ENUM(Imm_20_1,
				high = 20,
				low  = 1
			)
			SCOPED_ENUM(Offset,
				high = 18,
				low  = 0
			)
			SCOPED_ENUM(OffsetSignExtension,
				high = 29, // the offset has a width of 30 (and not 32) since the program_counter has a 30 bits width
				low  = 19
			)
		}
	}
}
