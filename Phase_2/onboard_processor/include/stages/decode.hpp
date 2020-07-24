#pragma once

/* ****************************************************************************
*    Header file for the decode stage
**************************************************************************** */

#include "common.hpp"
#include "components/register_map.hpp"
#include "stage_structs.hpp"

class DecodeStage {
	RegisterMap         register_map;
	instruction_token_t token;
public:
	DecodeStage();
	void interface(FetchToDecode& from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, DecodeToCommit* to_commit, bit_t* decode_ran);
};