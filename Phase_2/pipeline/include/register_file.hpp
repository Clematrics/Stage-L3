#pragma once

#include "../include/common.hpp"

#include "../include/circular_buffer.hpp"

struct RegisterEntry {
	word_t value;
	bool is_ready;
};

class RegisterFile {
	RegisterEntry registers[physical_register_count];

	physical_id_t map[architectural_register_count];
	CircularBuffer<physical_id_t, physical_register_count> free_list;
public:
	RegisterFile();
	void get_alias(const reg_t& id, physical_id_t* alias);
	void create_alias(const reg_t& id, physical_id_t* alias, bool* blocking);
	void free_alias(const physical_id_t& id);
	bool is_alias_ready(const physical_id_t& id);
	void read(const physical_id_t& id, word_t* read_value);
	void write(const physical_id_t& id, word_t write_value);
};