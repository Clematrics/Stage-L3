#pragma once

/* ****************************************************************************
*    Header file for the top function
**************************************************************************** */

#include "common.hpp"

#ifdef DBG_SYNTH
void processor(memory_t memory, TW(large_bool) run, TW(large_bool) stop, DebugInfo* dbg);
#else
void processor(memory_t memory, TW(large_bool) stop);
#endif