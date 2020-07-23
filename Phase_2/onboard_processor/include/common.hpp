#pragma once

/* ****************************************************************************
*    This file regroups all common types and constants used throughout the
*    implementation
**************************************************************************** */

#include "ap_int.h"

/* ****************************************************************************
*    Types with explicit size
**************************************************************************** */

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef signed char       int8_t;
typedef signed short      int16_t;
typedef signed int        int32_t;
typedef signed long long  int64_t;

/* ****************************************************************************
*    Special type declarations for the top function
**************************************************************************** */

// TW means two-way. A two-way type (an array of size 1 of a choosen type)
// allows to communicate with the CPU through the same
// address, and both can read and write at this address.
#define TW(type) tw_##type
#define DECL_TW_TYPE(type) typedef type TW(type)[1]

// A special type to exchange boolean information is used. Any two-way type with a
// size of 8 bits will make the synthesis fail, because of a bug to compute the width
// of the address
typedef uint32_t large_bool;
DECL_TW_TYPE(large_bool);

/* ****************************************************************************
*    Computation of the width of an address for accessing N cells
**************************************************************************** */

// Compute the width (number of bits) necessary to represent addresses in [0, N - 1]
template<int X>
struct WidthInternal {
	static const uint16_t N = WidthInternal<X / 2>::N + 1;
};

template<>
struct WidthInternal<0> {
	static const uint16_t N = 0;
};

template<int X>
struct Width {
	static const uint16_t Value = WidthInternal<X - 1>::N;
};

/* ****************************************************************************
*    Common types used by the implementation
**************************************************************************** */

// ..._bits  denotes the number of bits of something
// ..._size  denotes the number of bytes of something
// ..._words denotes the number of words of something
// ..._width denotes the number of bits necessary to represent all values of something

const uint8_t  byte_bits    = 8;                      // Number of bits in a byte
const uint8_t  word_size    = 4;                      // Number of bytes in a word
const uint8_t  word_bits    = word_size * byte_bits;  // Number of bits in a word

typedef ap_uint<1>             bit_t;
typedef ap_uint<byte_bits>     byte_t;
typedef ap_uint<word_bits>     word_t;
typedef ap_int<word_bits>      signed_word_t;
typedef ap_uint<2 * word_bits> double_word_t;

typedef ap_uint<word_bits - 2> program_counter_t;     // The program counter is aligned on 32 bits: the two lower bits of the address are ignored

/* ****************************************************************************
*    RISC-V implementation constants and types
**************************************************************************** */

const uint32_t memory_words = 32;
const uint32_t memory_size  = memory_words * word_size;

typedef word_t memory_t[memory_words];

const uint16_t physical_register_count      = 64;
const uint16_t architectural_register_count = 32;
#ifndef __SYNTHESIS__
static_assert(architectural_register_count <= physical_register_count);
#endif // __SYNTHESIS__

const uint32_t physical_register_count_width      = Width<physical_register_count>::Value;
const uint32_t architectural_register_count_width = Width<architectural_register_count>::Value;
typedef ap_uint<physical_register_count_width>      physical_reg_t;
typedef ap_uint<physical_register_count_width>      physical_reg_ptr_t;
typedef ap_uint<architectural_register_count_width> reg_t;

const uint16_t reorder_buffer_count = 16;
const uint32_t reorder_buffer_count_width         = Width<reorder_buffer_count>::Value;
typedef ap_uint<reorder_buffer_count_width>         reorder_buffer_ptr_t;

typedef ap_uint<reorder_buffer_count_width>         instruction_token_t;
const instruction_token_t initial_token           = 0;

const uint16_t issue_table_entries_count = reorder_buffer_count;
const uint32_t issue_table_ptr_width              = Width<issue_table_entries_count>::Value;
typedef ap_uint<issue_table_ptr_width>              issue_table_ptr_t;
