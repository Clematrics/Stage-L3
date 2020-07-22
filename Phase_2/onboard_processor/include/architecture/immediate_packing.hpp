#pragma once

#include "architecture/architecture.hpp"
#include "architecture/decoding.hpp"

using namespace Architecture::Type;

packed_immediate_t pack_I_immediate(const word_t& instruction);
packed_immediate_t pack_S_immediate(const word_t& instruction);
packed_immediate_t pack_B_immediate(const word_t& instruction);
packed_immediate_t pack_U_immediate(const word_t& instruction);
packed_immediate_t pack_J_immediate(const word_t& instruction);

program_counter_t extract_J_pc_offset(const word_t& instruction);

word_t unpack_I_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_S_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_B_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_U_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_J_immediate(const packed_immediate_t& packed_immediate);
word_t unpack_immediate(Type type, const packed_immediate_t& packed_immediate);