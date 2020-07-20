#pragma once

/* ****************************************************************************
*    Header file which defines the structures used by stages to communicate
**************************************************************************** */

#include "common.hpp"

struct FetchToDecode {
	bool              has_fetched;
	word_t            instruction;
	program_counter_t pc;
};

struct DecodeToFetch {
	bool              has_next_pc;
	program_counter_t next_pc;
};