#pragma once

/* ****************************************************************************
*    Header file for the decode stage
**************************************************************************** */

#include "common.hpp"
#include "inter_stage.hpp"

class DecodeStage {
public:
	DecodeStage();
	void interface(FetchToDecode& from_fetch, DecodeToFetch* to_fetch, bit_t* stop, bit_t* decode_ran);
};