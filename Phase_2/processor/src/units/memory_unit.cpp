#include "units/memory_unit.hpp"

#include "common.hpp"

#include "architecture/immediate_packing.hpp"

MemoryUnit::MemoryUnit() {

}

void MemoryUnit::interface(MemoryUnitInput input, MemoryUnitOutput* output) {
	word_t I_immediate = unpack_I_immediate(input.packed_immediate);
	word_t S_immediate = unpack_S_immediate(input.packed_immediate);

	output->valid_instruction = true;
	if (input.kind == Decoding::Kind::load) {
		output->operation = Operation::load;
		output->effective_address = input.src1_value + I_immediate;
		switch (input.func3) {
		case Decoding::Func3::lb  : output->size = Size::byte;         output->sign = Sign::signed_;   break;
		case Decoding::Func3::lh  : output->size = Size::half_word;    output->sign = Sign::signed_;   break;
		case Decoding::Func3::lw  : output->size = Size::word;         output->sign = Sign::signed_;   break;
		case Decoding::Func3::l011: output->valid_instruction = false;                                 break;
		case Decoding::Func3::lbu : output->size = Size::byte;         output->sign = Sign::unsigned_; break;
		case Decoding::Func3::lhu : output->size = Size::half_word;    output->sign = Sign::unsigned_; break;
		case Decoding::Func3::l110: output->valid_instruction = false;                                 break;
		case Decoding::Func3::l111: output->valid_instruction = false;                                 break;
		}
	}
	else if (input.kind == Decoding::Kind::store) {
		output->operation = Operation::store;
		output->effective_address = input.src1_value + S_immediate;
		switch (input.func3) {
		case Decoding::Func3::sb  : output->size = Size::byte;         break;
		case Decoding::Func3::sh  : output->size = Size::half_word;    break;
		case Decoding::Func3::sw  : output->size = Size::word;         break;
		case Decoding::Func3::s011: output->valid_instruction = false; break;
		case Decoding::Func3::s100: output->valid_instruction = false; break;
		case Decoding::Func3::s101: output->valid_instruction = false; break;
		case Decoding::Func3::s110: output->valid_instruction = false; break;
		case Decoding::Func3::s111: output->valid_instruction = false; break;
		}
	}
	else {
		output->valid_instruction = false;
	}
}