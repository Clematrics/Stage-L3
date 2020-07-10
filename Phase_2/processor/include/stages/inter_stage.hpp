#pragma once

#include "common.hpp"
#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Decoding::Kind;
using namespace Architecture::Type;


struct FetchToDecode {
	bool has_fetched_instruction;
	word_t instruction;
	program_counter_t program_counter;
};

struct DecodeToIssue {
	bool has_decoded_instruction;
	// decoded instruction and all informations
	Type type;
	Kind kind;
	func3_t func3;
	physical_reg_t src1;
	physical_reg_t src2;
	physical_reg_t dest;
	bool use_src1;
	bool use_src2;
	bool use_dest;
	bool is_func7_0b0000000;
	bool is_func7_0b0000001;
	bool is_func7_0b0100000;
	packed_immediate_t packed_immediate;
};

struct DecodeToFetch {
	bool block_fetch_stage;
	program_counter_t next_program_counter;
};

struct DecodeToCommit {
	bool has_decoded_instruction;
	// decoded instruction and all informations necessary for the ROB
	// TODO add ROB informations
};

struct IssueToDecode {
	bool block_decode_stage;
};

struct IssueToWriteBack {
	bool issue_instruction;
	// informations about the current instruction
	// TODO : add informations for the instruction, value of the registers
};

struct WriteBackToCommit {

};

struct WriteBackToFetch {

};

struct WriteBackToIssue {
	bool has_a_src_ready;
	physical_reg_t src_ready;
};