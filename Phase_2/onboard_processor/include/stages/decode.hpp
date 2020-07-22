#pragma once

/* ****************************************************************************
*    Header file for the decode stage
**************************************************************************** */

#include "common.hpp"
#include "components/register_map.hpp"
#include "inter_stage.hpp"

class DecodeStage {
	RegisterMap register_map;
public:
	DecodeStage();
	void interface(FetchToDecode& from_fetch, DecodeToFetch* to_fetch, DecodeToIssue* to_issue, bit_t* stop, bit_t* decode_ran);
};