#include "architecture/immediate_packing.hpp"

#include "architecture/decoding.hpp"
using namespace Decoding;

packed_immediate_t pack_I_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(I::InPackedImmediate::Imm_11_0::high, I::InPackedImmediate::Imm_11_0::low) = instruction(I::InInstruction::Imm_11_0::high, I::InInstruction::Imm_11_0::low);
	return immediate;
}

packed_immediate_t pack_S_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(S::InPackedImmediate::Imm_4_0::high,  S::InPackedImmediate::Imm_4_0::low)  = instruction(S::InInstruction::Imm_4_0::high , S::InInstruction::Imm_4_0::low);
	immediate(S::InPackedImmediate::Imm_11_5::high, S::InPackedImmediate::Imm_11_5::low) = instruction(S::InInstruction::Imm_11_5::high, S::InInstruction::Imm_11_5::low);
	return immediate;
}

packed_immediate_t pack_B_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(B::InPackedImmediate::Imm_4_1::high,  B::InPackedImmediate::Imm_4_1::low)  = instruction(B::InInstruction::Imm_4_1::high , B::InInstruction::Imm_4_1::low);
	immediate(B::InPackedImmediate::Imm_10_5::high, B::InPackedImmediate::Imm_10_5::low) = instruction(B::InInstruction::Imm_10_5::high, B::InInstruction::Imm_10_5::low);
	immediate.set_bit(B::InPackedImmediate::Imm_11::pos, instruction.get_bit(B::InInstruction::Imm_11::pos));
	immediate.set_bit(B::InPackedImmediate::Imm_12::pos, instruction.get_bit(B::InInstruction::Imm_12::pos));
	return immediate;
}

packed_immediate_t pack_U_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(U::InPackedImmediate::Imm_31_12::high, U::InPackedImmediate::Imm_31_12::low) = instruction(U::InInstruction::Imm_31_12::high, U::InInstruction::Imm_31_12::low);
	return immediate;
}

packed_immediate_t pack_J_immediate(const word_t& instruction) {
	packed_immediate_t immediate;
	immediate(J::InPackedImmediate::Imm_10_1::high, J::InPackedImmediate::Imm_10_1::low)   = instruction(J::InInstruction::Imm_10_1::high, J::InInstruction::Imm_10_1::low);
	immediate.set_bit(J::InPackedImmediate::Imm_11::pos, instruction.get_bit(J::InInstruction::Imm_11::pos));
	immediate(J::InPackedImmediate::Imm_19_12::high, J::InPackedImmediate::Imm_19_12::low) = instruction(J::InInstruction::Imm_19_12::high, J::InInstruction::Imm_19_12::low);
	immediate.set_bit(J::InPackedImmediate::Imm_20::pos, instruction.get_bit(J::InInstruction::Imm_20::pos));
	return immediate;
}

word_t unpack_I_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(11,  0) = packed_immediate(I::InPackedImmediate::Imm_11_0::high, I::InPackedImmediate::Imm_11_0::low);
	immediate(31, 12) = packed_immediate.get_bit(I::InPackedImmediate::Sign::pos) ? -1 : 0;
	return immediate;
}

word_t unpack_S_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(S::InUnpackedImmediate::Imm_11_0::high, S::InUnpackedImmediate::Imm_11_0::low) = packed_immediate(S::InPackedImmediate::Imm_11_0::high, S::InPackedImmediate::Imm_11_0::low);
	immediate(S::InUnpackedImmediate::SignExtension::high, S::InUnpackedImmediate::SignExtension::low) = packed_immediate.get_bit(S::InPackedImmediate::Sign::pos) ? -1 : 0;
	return immediate;
}

word_t unpack_B_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate.set_bit(B::InUnpackedImmediate::Imm_0::pos, B::InUnpackedImmediate::Imm_0::value);
	immediate(B::InUnpackedImmediate::Imm_12_1::high, B::InUnpackedImmediate::Imm_12_1::low) = packed_immediate(B::InPackedImmediate::Imm_12_1::high, B::InPackedImmediate::Imm_12_1::low);
	immediate(B::InUnpackedImmediate::SignExtension::high, B::InUnpackedImmediate::SignExtension::low) = packed_immediate.get_bit(S::InPackedImmediate::Sign::pos) ? -1 : 0;
	return immediate;
}

word_t unpack_U_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate(U::InUnpackedImmediate::Imm_11_0::high, U::InUnpackedImmediate::Imm_11_0::low) = U::InUnpackedImmediate::Imm_11_0::value;
	immediate(U::InUnpackedImmediate::Imm_31_12::high, U::InUnpackedImmediate::Imm_31_12::low) = packed_immediate(U::InPackedImmediate::Imm_31_12::high, U::InPackedImmediate::Imm_31_12::low);
	return immediate;
}

word_t unpack_J_immediate(const packed_immediate_t& packed_immediate) {
	word_t immediate;
	immediate.set_bit(J::InUnpackedImmediate::Imm_0::pos, J::InUnpackedImmediate::Imm_0::value);
	immediate(J::InUnpackedImmediate::Imm_20_1::high, J::InUnpackedImmediate::Imm_20_1::low) = packed_immediate(J::InPackedImmediate::Imm_20_1::high, J::InPackedImmediate::Imm_20_1::low);
	immediate(J::InUnpackedImmediate::SignExtension::high, J::InUnpackedImmediate::SignExtension::low) = packed_immediate.get_bit(J::InPackedImmediate::Sign::pos) ? -1 : 0;
	return immediate;
}

word_t unpack_immediate(Type type, const packed_immediate_t& packed_immediate) {
	switch (type) {
	case Architecture::Type::R: break;
	case Architecture::Type::I: return unpack_I_immediate(packed_immediate);
	case Architecture::Type::S: return unpack_S_immediate(packed_immediate);
	case Architecture::Type::B: return unpack_B_immediate(packed_immediate);
	case Architecture::Type::U: return unpack_U_immediate(packed_immediate);
	case Architecture::Type::J: return unpack_J_immediate(packed_immediate);
	case Architecture::Type::unknown_type: break;
	}
	return 0;
}