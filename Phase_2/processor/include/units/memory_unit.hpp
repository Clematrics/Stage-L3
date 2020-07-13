#pragma once

#include "common.hpp"

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Decoding::Kind;

enum Operation { load = 0, store = 1 };
enum Size { byte = 0, half_word = 1, word = 2 };
enum Sign { unsigned_ = 0, signed_ = 1 };

struct MemoryUnitInput {
	Kind kind;
	func3_t func3;
	word_t src1_value;
	word_t src2_value;
	packed_immediate_t packed_immediate;
};

struct MemoryUnitOutput {
	bool valid_instruction;
	word_t effective_address;
	Operation operation;
	Size size;
	Sign sign;
};

class MemoryUnit {
public:

	MemoryUnit();
	void interface(MemoryUnitInput input, MemoryUnitOutput* output);
};