#pragma once

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Architecture::Type;

struct BranchUnitInput {
	program_counter_t current_pc;
	func3_t func3;
	word_t src1_value;
	word_t src2_value;
	packed_immediate_t packed_immediate;
};

struct BranchUnitOutput {
	bool valid_instruction;
	program_counter_t next_pc;
};

class BranchUnit {
public:
	BranchUnit();
	void interface(BranchUnitInput input, BranchUnitOutput* output);
};