#pragma once

#include "common.hpp"
#include "architecture/architecture.hpp"
#include "debug/debug.hpp"

using namespace Architecture::InstructionCategory;

struct ComputeUnitInput {
	program_counter_t   pc;
	InstructionCategory category;
	func3_t             func3;
	bit_t               is_func7_0b0000000;
	bit_t               is_func7_0b0000001;
	bit_t               is_func7_0b0100000;
	word_t              src1_value;
	word_t              src2_value;
	packed_immediate_t  packed_immediate;
};

struct ComputeUnitOutput {
	bit_t             has_result;
	word_t            result;
	bit_t             has_next_pc;
	program_counter_t next_pc;
	bit_t             invalid;

	bit_t             is_load;
	bit_t             is_store;
	bit_t             is_byte_operation;
	bit_t             is_half_operation;
	bit_t             is_word_operation;
	bit_t             is_unsigned_extended;
	address_t         address;
};

class ComputeUnit {
public:
	ComputeUnit();
	void interface(ComputeUnitInput& input, ComputeUnitOutput* output);
private:
};