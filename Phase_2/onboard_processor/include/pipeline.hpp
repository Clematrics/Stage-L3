#pragma once

/* ****************************************************************************
*    Header file for the pipeline
**************************************************************************** */

#include "config.hpp"
#include "common.hpp"
#include "debug/debug.hpp"
#include "stages/fetch.hpp"
#include "stages/decode.hpp"
#include "stages/inter_stage.hpp"

/* ****************************************************************************
*    Macros to simulate the behavior of the pipeline on hardware
*    IS stands for Inter Stage
**************************************************************************** */
#ifndef __SYNTHESIS__
#define IS_IN(name)  in_##name
#define IS_OUT(name) out_##name
#define DECL_IS(type, name) type IS_IN(name); type IS_OUT(name)
#define TRANSFER_IS(name) IS_IN(name) = IS_OUT(name)
#else
#define IS_IN(name)  name
#define IS_OUT(name) name
#define DECL_IS(type, name) type name
#define TRANSFER_IS(name)
#endif

class Pipeline {
	// Stages
	FetchStage fetch_stage;
	DecodeStage decode_stage;

	// Inter stage structures
	DECL_IS(FetchToDecode, fetch_to_decode);
	DECL_IS(DecodeToFetch, decode_to_fetch);
public:
	Pipeline();

	#ifdef DBG_SYNTH
	void interface(memory_t memory, bit_t* stop, DebugInfo* dbg);
	#else
	void interface(memory_t memory, bit_t* stop);
	#endif
};