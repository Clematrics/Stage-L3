#pragma once

/* ****************************************************************************
*    Header file for the decode stage
**************************************************************************** */

#include "common.hpp"
#include "inter_stage.hpp"

class DecodeStage {
	bool locked;
public:
	DecodeStage();
	void interface(FetchToDecode& from_fetch, DecodeToFetch* to_fetch, large_bool* stop, bool* locked_dbg);
};