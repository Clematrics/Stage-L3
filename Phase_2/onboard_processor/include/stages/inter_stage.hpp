#pragma once

/* ****************************************************************************
*    Header file which defines the structures used by stages to communicate
**************************************************************************** */

#include "common.hpp"
#include "architecture/decoding.hpp"

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

	bit_t              invalid_instruction; // If unknown instruction
};

struct DecodeToCommit {
	bit_t               add_to_rob;
	instruction_token_t token;
	bit_t               invalid_instruction;
};

struct IssueToWriteBack {

};

struct WriteBackToIssue {

};

struct CommitToIssue {

};
