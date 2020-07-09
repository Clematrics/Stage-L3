#pragma once

#include "common.hpp"
#include "components/circular_buffer.hpp"

class RegisterMap {
	physical_reg_t map[architectural_register_count];
	CircularBuffer<physical_reg_t, physical_register_count, physical_reg_ptr_t> free_list;
public:
	RegisterMap();
	void get_alias(const reg_t& id, physical_reg_t* alias);
	void create_alias(const reg_t& id, physical_reg_t* alias, bool* blocking);
	void free_alias(const physical_reg_t& id);
};