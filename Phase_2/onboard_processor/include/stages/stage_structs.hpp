#pragma once

/* ****************************************************************************
*    Header file which defines the structures used by stages to communicate
*    and structures that are used by stages to store information
**************************************************************************** */

#include "common.hpp"
#include "architecture/decoding.hpp"

/* ****************************************************************************
*    Intra-stage structures
**************************************************************************** */

struct IssueTableEntry {
	bit_t          used;
	bit_t          use_src1;
	bit_t          use_src2;
	bit_t          src1_ready;
	bit_t          src2_ready;
	physical_reg_t src1;
	physical_reg_t src2;
};

struct ReorderBufferEntry {
	instruction_token_t token; // is not useful, since the token is the same as the entry's index in the ROB
	bit_t               done;
	bit_t               invalid;
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
	bit_t              has_decoded_instruction;
	program_counter_t  pc;
	func3_t            func3;
	bit_t              is_func7_0b0000000;
	bit_t              is_func7_0b0000001;
	bit_t              is_func7_0b0100000;
	packed_immediate_t packed_immediate;

	bit_t              use_dest;
	bit_t              use_src1;
	bit_t              use_src2;
	physical_reg_t     dest;
	physical_reg_t     src1;
	physical_reg_t     src2;

	bit_t              invalid; // If unknown instruction
};

struct DecodeToCommit {
	bit_t               add_to_rob;
	instruction_token_t token;
	bit_t               invalid;
};

struct IssueToWriteBack {

};

struct WriteBackToIssue {
	bool           has_a_src_ready;
	physical_reg_t src_ready;
	word_t         result;
};

struct CommitToCommit {
	bit_t              rob_was_empty;
	ReorderBufferEntry previous_first_entry;
};

struct CommitToIssue {

};
