#pragma once

/* ****************************************************************************
*    Header file which defines the structures used by stages to communicate
**************************************************************************** */

#include "common.hpp"

struct FetchToDecode {
	bit_t             has_fetched;
	word_t            instruction;
	program_counter_t pc;
};

struct DecodeToFetch {
	bit_t             has_next_pc;
	program_counter_t next_pc;
};