#pragma once

/* ****************************************************************************
*    Header file which defines the structures used by stages to communicate
*    and structures that are used by stages to store information
**************************************************************************** */

#include "common.hpp"
#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Architecture::InstructionCategory;

/* ****************************************************************************
*    Intra-stage structures
**************************************************************************** */

struct IssueTableEntry {
	bit_t               used;
	instruction_token_t token;

	program_counter_t   pc;
	InstructionCategory category;
	func3_t             func3;
	bit_t               is_func7_0b0000000;
	bit_t               is_func7_0b0000001;
	bit_t               is_func7_0b0100000;
	packed_immediate_t  packed_immediate;

	bit_t               use_dest;
	bit_t               use_src1;
	bit_t               use_src2;
	physical_reg_t      dest;
	physical_reg_t      src1;
	physical_reg_t      src2;
	bit_t               src1_ready;
	bit_t               src2_ready;
};

struct ReorderBufferEntry {
	instruction_token_t token; // is not useful, since the token is the same as the entry's index in the ROB
	bit_t               ready;
	bit_t               invalid;

	bit_t               is_load;
	bit_t               is_store;
	bit_t               is_byte_operation;
	bit_t               is_half_operation;
	bit_t               is_word_operation;
	bit_t               is_unsigned_extended;
	physical_reg_t      load_dest;
	word_t              value;
	address_t           address;
};

/* ****************************************************************************
*    Inter-stage structures
**************************************************************************** */

struct FetchToDecode {
	bit_t             has_fetched;
	word_t            instruction;
	program_counter_t pc;
};

struct DecodeToFetch {
	bit_t             has_next_pc;
	program_counter_t next_pc;
};

struct DecodeToIssue {
	bit_t               has_decoded_instruction;
	instruction_token_t token;
	program_counter_t   pc;
	InstructionCategory category;
	func3_t             func3;
	bit_t               is_func7_0b0000000;
	bit_t               is_func7_0b0000001;
	bit_t               is_func7_0b0100000;
	packed_immediate_t  packed_immediate;

	bit_t               use_dest;
	bit_t               use_src1;
	bit_t               use_src2;
	physical_reg_t      dest;
	physical_reg_t      src1;
	physical_reg_t      src2;
	bit_t               src1_ready;
	bit_t               src2_ready;
};

struct DecodeToCommit {
	bit_t               add_to_rob;
	instruction_token_t token;
	bit_t               invalid;
};

struct IssueToWriteBack {
	bit_t               executed_instruction;
	instruction_token_t token;

	bit_t               has_result;
	word_t              result;
	physical_reg_t      dest;
	bit_t               has_next_pc;
	program_counter_t   next_pc;
	bit_t               invalid;

	bit_t               is_load;
	bit_t               is_store;
	bit_t               is_byte_operation;
	bit_t               is_half_operation;
	bit_t               is_word_operation;
	bit_t               is_unsigned_extended;
	address_t           address;
};

struct WriteBackToFetch {
	bit_t             has_next_pc;
	program_counter_t next_pc;
};

struct WriteBackToIssue {
	bit_t          has_a_src_ready;
	physical_reg_t src_ready;
	word_t         result;
};

struct WriteBackToCommit {
	bit_t               executed_instruction;
	instruction_token_t token;
	bit_t               invalid;

	bit_t               is_load;
	bit_t               is_store;
	bit_t               is_byte_operation;
	bit_t               is_half_operation;
	bit_t               is_word_operation;
	bit_t               is_unsigned_extended;
	physical_reg_t      load_dest;
	word_t              value;
	address_t           address;
};

struct CommitToIssue {
	bit_t          has_loaded_value;
	physical_reg_t dest;
	word_t         value;
};

struct CommitToCommit {
	bit_t              rob_was_empty;
	ReorderBufferEntry previous_first_entry;
};
