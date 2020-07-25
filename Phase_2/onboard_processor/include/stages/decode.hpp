#pragma once

/* ****************************************************************************
*    Header file for the decode stage
**************************************************************************** */

#include "common.hpp"
#include "components/register_map.hpp"
#include "stage_structs.hpp"

class DecodeStage {
	physical_reg_t register_map[architectural_register_count];
	physical_reg_t free_aliases[physical_register_count];
	physical_reg_ptr_t free_aliases_bot;
	physical_reg_ptr_t free_aliases_top;
	bit_t free_aliases_empty;
	bit_t free_aliases_full;

	void free_aliases_push_back(physical_reg_t entry);
	void free_aliases_pop();

	void get_register_alias(const reg_t& id, physical_reg_t* alias);
	void create_register_alias(const reg_t& id, physical_reg_t* alias, bit_t* blocking);
	void free_register_alias(const physical_reg_t& id);

	instruction_token_t token;
public:
	DecodeStage();
	void interface(FetchToDecode& from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, DecodeToCommit* to_commit, bit_t* decode_ran);
};