#pragma once

#include "ap_int.h"

// Types for constants
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

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