#pragma once

/* ****************************************************************************
*    Header file for the top function
**************************************************************************** */

#include "common.hpp"

#ifdef DBG_SYNTH
void processor(memory_t memory, TW(large_bool) run, bit_t* stopped, DebugInfo* dbg);
#else
void processor(memory_t memory, bit_t* stopped);
#endif