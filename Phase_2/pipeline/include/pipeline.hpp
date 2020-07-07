#pragma once

#include "../include/common.hpp"
#include "../include/fetch.hpp"
#include "../include/decode.hpp"
#include "../include/issue.hpp"
#include "../include/register_file.hpp"

struct FetchToDecode {
	bool fetched;
	word_t instruction;
	word_t program_counter;
};

struct DecodeToFetch {
	word_t next_program_counter;
	bool block_fetch;
};

// to enqueue the instruction into the ROB
struct DecodeToCommit {
	bool enqueue;
	DecodedInstruction decoded_instruction;
};

struct DecodeToIssue {
	bool decoded;
	DecodedInstruction decoded_instruction;
	// add something for the renaming ?
};

//
struct WritebackToIssue {
	bool write_result;
	physical_id_t destination;
	word_t value;
};

class Pipeline {
	Fetch fetch_stage;
	Decode decode_stage;
	Issue issue_stage;
	DecodedInstruction decoded;
	word_t instruction;
	word_t program_counter;
	word_t next_program_counter;
public:
	Pipeline(word_t initial_program_counter = 0);
	void pipeline(memory_t memory, bit_t* stop);
};

void pipeline(memory_t memory);
