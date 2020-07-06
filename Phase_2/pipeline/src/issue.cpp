#include "../include/issue.hpp"

Issue::Issue()
{
	for (uint16_t i = 0; i < issue_table_entries_count; i++) {
		#pragma HLS UNROLL
		free_list.push_back(i);
	}
}

void Issue::issue() {
	
}