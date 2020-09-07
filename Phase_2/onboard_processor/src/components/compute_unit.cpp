#include "components/compute_unit.hpp"

#include "architecture/immediate_packing.hpp"

ComputeUnit::ComputeUnit() {}

void ComputeUnit::interface(ComputeUnitInput& input, ComputeUnitOutput* output) {
	// TODO : do not make all computations simultaneously to decrease power usage

	word_t immediate_i = unpack_I_immediate(input.packed_immediate);
	word_t immediate_s = unpack_S_immediate(input.packed_immediate);
	word_t immediate_b = unpack_B_immediate(input.packed_immediate);
	word_t immediate_u = unpack_U_immediate(input.packed_immediate);
	word_t immediate_j = unpack_J_immediate(input.packed_immediate);

	// lui
	word_t lui_res                     = immediate_u;
	// auipc
	word_t auipc_res                   = immediate_u + (input.pc << 2);
	// jal
	word_t jal_res                     = input.pc + 1; // this is a potentially useless back-and-forth. Could the value be added to the registers' file directly?
	// jalr
	word_t jalr_res                    = input.pc + 1;
	word_t jalr_target_address         = input.src1_value + immediate_i;
	jalr_target_address.set_bit(0, false); // TODO : convert the constant to a symbol
	program_counter_t jalr_target_pc   = jalr_target_address(31, 2); // TODO :  convert the constants to symbols
	bit_t jalr_invalid                 = jalr_target_address.get_bit(1); // TODO : convert the constant to a symbol
	// branches
	word_t branch_target_address       = input.pc + immediate_b;
	program_counter_t branch_next_pc   = input.pc + 1;
	program_counter_t branch_target_pc = branch_target_address(31, 2); //  TODO : convert the constants to symbols
	bit_t branch_invalid               = branch_target_address.get_bit(1); // TODO : convert the constant to a symbol
	// beq
	bit_t beq_take_branch              = input.src1_value == input.src2_value;
	// bne
	bit_t bne_take_branch              = !beq_take_branch;
	// blt
	bit_t blt_take_branch              = ((signed_word_t)input.src1_value) < ((signed_word_t)input.src2_value);
	// bge
	bit_t bge_take_branch              = !blt_take_branch;
	// bltu
	bit_t bltu_take_branch             = input.src1_value < input.src2_value;
	// bgeu
	bit_t bgeu_take_branch             = !bltu_take_branch;
	// load
	word_t load_address                = input.src1_value + immediate_i;
	// store
	word_t store_address               = input.src1_value + immediate_s;
	// addi
	word_t addi_res                    = input.src1_value + immediate_i;
	// slti
	word_t slti_res                    = (((signed_word_t)input.src1_value) < ((signed_word_t)immediate_i)) ? 1 : 0;
	// sltiu
	word_t sltiu_res                   = (input.src1_value < immediate_i) ? 1 : 0;
	// xori
	word_t xori_res                    = input.src1_value ^ immediate_i;
	// ori
	word_t ori_res                     = input.src1_value | immediate_i;
	// andi
	word_t andi_res                    = input.src1_value & immediate_i;
	// slli
	word_t slli_res                    = input.src1_value << immediate_i(4, 0);
	// srli
	word_t srli_res                    = input.src1_value >> immediate_i(4, 0);
	// srai
	word_t srai_res                    = ((signed_word_t)input.src1_value) >> immediate_i(4, 0);
	// add
	word_t add_res                     = input.src1_value + input.src2_value;
	// sub
	word_t sub_res                     = input.src1_value - input.src2_value;
	// sll
	word_t sll_res                     = input.src1_value << input.src2_value(4, 0);
	// slt
	word_t slt_res                     = (((signed_word_t)input.src1_value) < ((signed_word_t)input.src2_value)) ? 1 : 0;
	// sltu
	word_t sltu_res                    = (input.src1_value < input.src2_value) ? 1 : 0;
	// xor
	word_t xor_res                     = input.src1_value ^ input.src2_value;
	// srl
	word_t srl_res                     = input.src1_value >> input.src2_value(4, 0);
	// sra
	word_t sra_res                     = ((signed_word_t)input.src1_value) >> input.src2_value(4, 0);
	// or
	word_t or_res                      = input.src1_value | input.src2_value;
	// and
	word_t and_res                     = input.src1_value & input.src2_value;
	// fence
	// ecall
	// ebreak

	output->has_result  = false;
	output->has_next_pc = false;
	output->invalid     = false;

	output->is_load              = false;
	output->is_store             = false;
	output->is_byte_operation    = false;
	output->is_half_operation    = false;
	output->is_word_operation    = false;
	output->is_unsigned_extended = false;

	switch (input.category) {
	case Architecture::InstructionCategory::load:
		output->is_load = true;
		output->address = load_address(31, 2);
		switch (input.func3) {
		case Decoding::Func3::load_lb:  output->is_byte_operation = true; break;
		case Decoding::Func3::load_lh:  output->is_half_operation = true; break;
		case Decoding::Func3::load_lw:  output->is_word_operation = true; break;
		case Decoding::Func3::load_011: output->invalid = true;           break;
		case Decoding::Func3::load_lbu: output->is_byte_operation = true; output->is_unsigned_extended = true; break;
		case Decoding::Func3::load_lhu: output->is_half_operation = true; output->is_unsigned_extended = true; break;
		case Decoding::Func3::load_110: output->invalid = true;           break;
		case Decoding::Func3::load_111: output->invalid = true;           break;
		}
		break;
	case Architecture::InstructionCategory::fence:
		// do nothing
		break;
	case Architecture::InstructionCategory::alui:
		output->has_result = true;
		switch (input.func3) {
		case Decoding::Func3::alui_addi:       output->result  = addi_res;  break;
		case Decoding::Func3::alui_slli:
			     if (input.is_func7_0b0000000) output->result  = slli_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alui_slti:       output->result  = slti_res;  break;
		case Decoding::Func3::alui_sltiu:      output->result  = sltiu_res; break;
		case Decoding::Func3::alui_xori:       output->result  = xori_res;  break;
		case Decoding::Func3::alui_srli_srai:
			     if (input.is_func7_0b0000000) output->result  = srli_res;
			else if (input.is_func7_0b0100000) output->result  = srai_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alui_ori:        output->result  = ori_res;   break;
		case Decoding::Func3::alui_andi:       output->result  = andi_res;  break;
		}
		break;
	case Architecture::InstructionCategory::auipc:
		output->has_result = true;
		output->result     = auipc_res;
		break;
	case Architecture::InstructionCategory::store:
		if (input.func3.get_bit(2)) {
			output->invalid = true;
		} else {
			output->is_store = true;
			output->address  = store_address(31, 2);
			switch (input.func3) {
			case Decoding::Func3::store_sb:  output->is_byte_operation = true; break;
			case Decoding::Func3::store_sh:  output->is_half_operation = true; break;
			case Decoding::Func3::store_sw:  output->is_word_operation = true; break;
			case Decoding::Func3::store_011: output->invalid = true;           break;
			}
		}
		break;
	case Architecture::InstructionCategory::alu:
		output->has_result = true;
		switch (input.func3) {
		case Decoding::Func3::alu_add_sub_mul:
			     if (input.is_func7_0b0000000) output->result  = add_res;
			else if (input.is_func7_0b0100000) output->result  = sub_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alu_sll_mulh:
			     if (input.is_func7_0b0000000) output->result  = sll_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alu_slt_mulhsu:
			     if (input.is_func7_0b0000000) output->result  = slt_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alu_sltu_mulhu:
			     if (input.is_func7_0b0000000) output->result  = sltu_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alu_xor_div:
			     if (input.is_func7_0b0000000) output->result  = xor_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alu_srl_sra_divu:
			     if (input.is_func7_0b0000000) output->result  = srl_res;
			else if (input.is_func7_0b0100000) output->result  = sra_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alu_or_rem:
			     if (input.is_func7_0b0000000) output->result  = or_res;
			else                               output->invalid = true;      break;
		case Decoding::Func3::alu_and_remu:
			     if (input.is_func7_0b0000000) output->result  = and_res;
			else                               output->invalid = true;      break;
		}
		break;
	case Architecture::InstructionCategory::lui:
		output->has_result = true;
		output->result     = lui_res;
		break;
	case Architecture::InstructionCategory::branch: {
		output->has_next_pc = true;
		bit_t take_branch   = false;
		switch (input.func3) {
		case Decoding::Func3::branch_beq:  take_branch = beq_take_branch;  break;
		case Decoding::Func3::branch_bne:  take_branch = bne_take_branch;  break;
		case Decoding::Func3::branch_010:  output->invalid = true;         break;
		case Decoding::Func3::branch_011:  output->invalid = true;         break;
		case Decoding::Func3::branch_blt:  take_branch = blt_take_branch;  break;
		case Decoding::Func3::branch_bge:  take_branch = bge_take_branch;  break;
		case Decoding::Func3::branch_bltu: take_branch = bltu_take_branch; break;
		case Decoding::Func3::branch_bgeu: take_branch = bgeu_take_branch; break;
		}
		output->next_pc  = take_branch ? branch_target_pc : branch_next_pc;
		output->invalid |= take_branch & branch_invalid;                    // the instruction is invalid only if the branch is taken
		break;
	}
	case Architecture::InstructionCategory::jal:
		output->has_result = true;
		output->result     = jal_res;
		break;
	case Architecture::InstructionCategory::jalr:
		output->has_result  = true;
		output->result      = jalr_res;
		output->has_next_pc = jalr_target_pc;
		output->invalid     = jalr_invalid;
		break;
	case Architecture::InstructionCategory::system:
		if (immediate_i(11, 1) == 0 && input.func3 == 0) ; // valid, either ecall or ebreak, do nothing
		else output->invalid = true;
		break;
	}
}
