#pragma once

#include "../include/common.hpp"

class RegisterFile {
	word_t registers[physical_register_count];
	physical_id_t free_list[physical_register_count];
	physical_id_t free_list_bot_ptr;
	physical_id_t free_list_top_ptr;

	physical_id_t map[architectural_register_count];
public:
	RegisterFile();
	void get_alias(const reg_t& id, physical_id_t* alias);
	void create_alias(const reg_t& id, physical_id_t* alias, bool* blocking);
	void free_alias(const reg_t& id);
	void read(bool read, const reg_t& id, word_t* read_value);
	void write(bool write, const reg_t& id, word_t write_value);
};