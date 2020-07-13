#pragma once

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Architecture::Type;
using namespace Decoding::Kind;

struct AluInput {
	Kind kind;
	func3_t func3;
	bool is_func7_0b0000000;
	bool is_func7_0b0000001;
	bool is_func7_0b0100000;
	word_t src1_value;
	word_t src2_value;
	packed_immediate_t packed_immediate;
	program_counter_t current_pc;
};

struct AluOutput {
	bool valid_instruction;
	word_t result;
	bool has_next_pc;
	program_counter_t next_pc;
};

class Alu {
public:
	Alu();
	void interface(AluInput input, AluOutput* output);
};