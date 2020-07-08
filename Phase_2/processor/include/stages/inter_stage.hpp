#pragma once

#include "common.hpp"

struct FetchToDecode {
	bool has_fetched_instruction;
	word_t instruction;
	word_t program_counter;
};

struct DecodeToIssue {
	bool has_decoded_instruction;
	// decoded instruction and all informations
};

struct DecodeToFetch {
	bool block_fetch_stage;
	word_t next_program_counter;
};

struct DecodeToCommit {
	bool has_decoded_instruction;
	// decoded instruction and all informations necessary for the ROB
};

struct IssueToWriteBack {

};

struct WriteBackToCommit {

};

struct WriteBackToFetch {

};

struct WriteBackToIssue {

};