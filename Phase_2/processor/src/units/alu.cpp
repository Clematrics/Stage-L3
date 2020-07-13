#include "units/alu.hpp"

#include "architecture/immediate_packing.hpp"

Alu::Alu() {}

void Alu::interface(AluInput input, AluOutput* output) {
	word_t U_immediate = unpack_U_immediate(input.packed_immediate);
	word_t I_immediate = unpack_I_immediate(input.packed_immediate);

	signed_word_t signed_src1 = input.src1_value;
	signed_word_t signed_src2 = input.src2_value;
	signed_word_t signed_I_immediate = I_immediate;

	output->valid_instruction = true;
	output->has_next_pc       = false;
	switch(input.kind) {
	case Decoding::Kind::alu: {
		double_word_t mul_signed_signed     = signed_src1 * signed_src2;
		double_word_t mul_signed_unsigned   = signed_src1 * input.src2_value;
		double_word_t mul_unsigned_unsigned = input.src1_value * input.src2_value;

		word_t div_signed   = signed_src1 / signed_src2;
		word_t div_unsigned = input.src1_value / input.src2_value;
		word_t rem_signed   = signed_src1 % signed_src2;
		word_t rem_unsigned = input.src1_value % input.src2_value;

		// m2Lm1  = -2^(L-1)
		// _2L_m1 =  2^L - 1
		bool is_divisor_null      = input.src2_value.is_zero();
		bool is_divisor_minus_one = (~signed_src2).is_zero();
		bool is_dividend_m2Lm1    = input.src2_value.sign() && input.src2_value(30, 0).operator ap_int_base<32, false>().is_zero();
		bool is_signed_overflow   = is_divisor_minus_one && is_dividend_m2Lm1;
		       word_t _2L_m1      = 0xFFFFFFFF;
		signed_word_t m2Lm1       = 0x80000000;
		// TODO : check if the default behavior of division and remainder are by default identical to the riscv spec

		switch (input.func3) {
		case Decoding::Func3::add_sub_mul  :
			if      (input.is_func7_0b0000000) /* add  */ output->result = input.src1_value + input.src2_value;
			else if (input.is_func7_0b0100000) /* sub  */ output->result = input.src1_value - input.src2_value;
			else if (input.is_func7_0b0000001) /* mul  */ output->result = mul_signed_signed(31, 0);
			else                               output->valid_instruction = false;
			break;
		case Decoding::Func3::sll_mulh     :
			if      (input.is_func7_0b0000000) /* sll  */ output->result = input.src1_value << input.src2_value(4, 0);
			else if (input.is_func7_0b0000001) /* mulh */ output->result = mul_signed_signed(63, 32);
			else                               output->valid_instruction = false;
			break;
		case Decoding::Func3::slt_mulhsu   :
			if      (input.is_func7_0b0000000) /* slt    */ output->result = signed_src1 < signed_src2 ? 1 : 0;
			else if (input.is_func7_0b0000001) /* mulhsu */ output->result = mul_signed_unsigned(63, 32);
			else                               output->valid_instruction = false;
			break;
		case Decoding::Func3::sltu_mulhu   :
			if      (input.is_func7_0b0000000) /* sltu  */ output->result = input.src1_value < input.src2_value ? 1 : 0;
			else if (input.is_func7_0b0000001) /* mulhu */ output->result = mul_unsigned_unsigned(63, 32);
			else                               output->valid_instruction = false;
			break;
		case Decoding::Func3::xor_div      :
			if      (input.is_func7_0b0000000) /* xor  */ output->result = input.src1_value ^ input.src2_value;
			else if (input.is_func7_0b0000001) /* div  */ output->result = is_signed_overflow ? static_cast<word_t>(m2Lm1) : (is_divisor_null ? static_cast<word_t>(-1) : div_signed);
			else                               output->valid_instruction = false;
			break;
		case Decoding::Func3::srl_sra_divu :
			if      (input.is_func7_0b0000000) /* srl  */ output->result = input.src1_value >> input.src2_value(4, 0);
			else if (input.is_func7_0b0100000) /* sra  */ output->result = signed_src1 >> input.src2_value(4, 0);
			else if (input.is_func7_0b0000001) /* divu */ output->result = is_divisor_null ? _2L_m1 : div_unsigned;
			else                               output->valid_instruction = false;
			break;
		case Decoding::Func3::or_rem       :
			if      (input.is_func7_0b0000000) /* or   */ output->result = input.src1_value | input.src2_value;
			else if (input.is_func7_0b0000001) /* rem  */ output->result = is_signed_overflow ? static_cast<word_t>(0) : (is_divisor_null ? static_cast<word_t>(signed_src1) : rem_signed);
			else                               output->valid_instruction = false;
			break;
		case Decoding::Func3::and_remu     :
			if      (input.is_func7_0b0000000) /* and  */ output->result = input.src1_value & input.src2_value;
			else if (input.is_func7_0b0000001) /* remu */ output->result = is_divisor_null ? static_cast<word_t>(signed_src1) : rem_unsigned;
			else                               output->valid_instruction = false;
			break;
		}
		break;
		}
	case Decoding::Kind::alu_immediate: {
		switch (input.func3) {
		case Decoding::Func3::addi     :       /* addi */ output->result = input.src1_value + I_immediate; break;
		case Decoding::Func3::slli     :
			if      (input.is_func7_0b0000000) /* slli */ output->result = input.src1_value << I_immediate(4,0);
			else                               output->valid_instruction = false;
		case Decoding::Func3::slti     :      /* slti  */ output->result = signed_src1 < signed_I_immediate ? 1 : 0;
		case Decoding::Func3::sltiu    :      /* sltiu */ output->result = input.src1_value < I_immediate ? 1 : 0;
		case Decoding::Func3::xori     :      /* xori  */ output->result = input.src1_value ^ I_immediate;
		case Decoding::Func3::srli_srai:
			if      (input.is_func7_0b0000000) /* srli */ output->result = input.src1_value >> I_immediate(4,0);
			else if (input.is_func7_0b0100000) /* srai */ output->result = signed_src1 >> I_immediate(4,0);
			else                               output->valid_instruction = false;
		case Decoding::Func3::ori      :       /* ori  */ output->result = input.src1_value | I_immediate;
		case Decoding::Func3::andi     :       /* andi */ output->result = input.src1_value & I_immediate;
		}
		break;
		}
	case Decoding::Kind::load:
		output->valid_instruction = false;
		break;
	case Decoding::Kind::store:
		output->valid_instruction = false;
		break;
	case Decoding::Kind::branch:
		output->valid_instruction = false;
		break;
	case Decoding::Kind::fence:
		output->valid_instruction = false;
		break;
	case Decoding::Kind::lui:   output->result = U_immediate;                    break;
	case Decoding::Kind::auipc:	output->result = U_immediate + input.current_pc; break;
	case Decoding::Kind::jal:
		output->valid_instruction = false;
		break;
	case Decoding::Kind::jalr:
		if (input.func3 == Decoding::Func3::jalr) {
			word_t jump_offset = unpack_I_immediate(input.packed_immediate);

			output->has_next_pc = true;
			output->next_pc = input.src1_value + jump_offset;
			output->next_pc.set_bit(0, false);
			output->result  = input.current_pc + 1;
		}
		else {
			output->valid_instruction = false;
		}
		break;
	case Decoding::Kind::system:
		output->valid_instruction = false;
		break;
	case Decoding::Kind::unknown_kind:
		output->valid_instruction = false;
		break;
	}
}