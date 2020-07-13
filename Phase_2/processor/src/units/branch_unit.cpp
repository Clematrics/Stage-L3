#include "units/branch_unit.hpp"

#include "architecture/decoding.hpp"
#include "architecture/immediate_packing.hpp"

using namespace Decoding;

BranchUnit::BranchUnit() {}

void BranchUnit::interface(BranchUnitInput input, BranchUnitOutput* output) {
	// Computing the next program counter. Needs the current one
	word_t branch_offset = unpack_B_immediate(input.packed_immediate);
	signed_word_t signed_src1 = input.src1_value;
	signed_word_t signed_src2 = input.src2_value;

	word_t    branch_addr = input.current_pc + branch_offset;
	word_t no_branch_addr = input.current_pc + 1;

	bool equal         = input.src1_value == input.src2_value;
	bool less          = signed_src1 < signed_src2;
	bool less_unsigned = input.src1_value < input.src2_value;

	output->valid_instruction = true;
	switch (input.func3) {
	case Func3::beq:  output->next_pc = equal ? branch_addr : no_branch_addr;         break;
	case Func3::bne:  output->next_pc = equal ? no_branch_addr : branch_addr;         break;
	case Func3::b010: output->valid_instruction = false; /* invalid func3 */          break;
	case Func3::b011: output->valid_instruction = false; /* invalid func3 */          break;
	case Func3::blt:  output->next_pc = less ? branch_addr : no_branch_addr;          break;
	case Func3::bge:  output->next_pc = less ? no_branch_addr : branch_addr;          break;
	case Func3::bltu: output->next_pc = less_unsigned ? branch_addr : no_branch_addr; break;
	case Func3::bgeu: output->next_pc = less_unsigned ? no_branch_addr : branch_addr; break;
	}
}