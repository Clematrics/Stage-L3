#pragma once

#include "ap_int.h"

// Types for constants
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

// Compute the width (number of bits) necessary to represent [0, N - 1]
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

// ..._bits denotes the number of bits of something
// ..._size denotes the number of bytes of something
// ..._words denots the number of words of something

const uint8_t  byte_bits    = 8;                      // Number of bits in a byte
const uint8_t  word_size    = 4;                      // Number of bytes in a word
const uint8_t  word_bits    = word_size * byte_bits;  // Number of bits in a word

typedef ap_uint<1>         bit_t;
typedef ap_uint<byte_bits> byte_t;
typedef ap_uint<word_bits> word_t;

const uint32_t memory_words = 16;
const uint32_t memory_size  = memory_words * word_size;

typedef word_t memory_t[memory_words];

const uint16_t physical_register_count      = 64;
const uint16_t architectural_register_count = 32;

const uint32_t physical_register_id_width      = Width<physical_register_count>::Value;
const uint32_t architectural_register_id_width = Width<architectural_register_count>::Value;
typedef ap_uint<physical_register_id_width>      physical_id_t;
typedef ap_uint<architectural_register_id_width> reg_t;

const uint16_t reorder_buffer_count = 16;
const uint32_t reorder_buffer_count_width      = Width<reorder_buffer_count>::Value;
typedef ap_uint<reorder_buffer_count_width>      reorder_buffer_ptr_t;

