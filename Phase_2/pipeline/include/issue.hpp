#pragma once

#include "../include/decode.hpp"
#include "../include/circular_buffer.hpp"
#include "../include/register_file.hpp"

class Issue {
	bool active;
	RegisterFile register_file;

	DecodedInstruction table[issue_table_entries_count];

	CircularBuffer<issue_table_ptr_t, issue_table_entries_count> free_list;

public:
	Issue();
	void issue();
};