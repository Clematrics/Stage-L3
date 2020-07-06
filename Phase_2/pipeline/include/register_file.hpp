#pragma once

#include "../include/common.hpp"

#include "../include/circular_buffer.hpp"

class RegisterFile {
	word_t registers[physical_register_count];

	physical_id_t map[architectural_register_count];
	CircularBuffer<physical_id_t, physical_register_count> free_list;
public:
	RegisterFile();
	void get_alias(const reg_t& id, physical_id_t* alias);
	void create_alias(const reg_t& id, physical_id_t* alias, bool* blocking);
	void free_alias(const physical_id_t& id);
	void read(bool read, const reg_t& id, word_t* read_value);
	void write(bool write, const reg_t& id, word_t write_value);
};