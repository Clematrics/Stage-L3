#pragma once

#include "../include/common.hpp"

const uint32_t memory_words = 4096;
const uint8_t  word_size    = sizeof(word_t);
const uint32_t memory_bytes = memory_words * word_size;

typedef uint8_t memory_t[memory_bytes];
