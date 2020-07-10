#pragma once

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Architecture::Type;

struct AluInput {
	bool has_request;
	Type type;
	func3_t func3;
	bool is_func7_0b0000000;
	bool is_func7_0b0000001;
	bool is_func7_0b0100000;
	word_t src1_value;
	word_t src2_value;
	packed_immediate_t packed_immediate;
};

struct AluOutput {
	bool is_busy;
	bool has_result;
	word_t result;
	physical_reg_t destination;
};

class Alu {
public:
	Alu();
	void interface(AluInput input, AluOutput* output);
};