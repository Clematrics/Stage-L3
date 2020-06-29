#include "../include/register_file.hpp"

RegisterFile::RegisterFile() {
	for (reg_t id = 0; id < architectural_register_count; id++) {
		#pragma HLS UNROLL
		map[id] = id;
	}
	for (physical_id_t id = architectural_register_count; id < physical_register_count; id++) {
		#pragma HLS UNROLL
		free_list[id - architectural_register_count] = id;
	}
	free_list_bot_ptr = 0;
	free_list_top_ptr = physical_register_count - architectural_register_count;
}

void RegisterFile::get_alias(const reg_t& id, physical_id_t* alias) {
	*alias = map[id];
}

void RegisterFile::create_alias(const reg_t& id, physical_id_t* alias, bool* blocking) {
	if (free_list_bot_ptr == free_list_top_ptr) {
		*blocking = true;
	}
	else {
		physical_id_t new_alias = free_list[free_list_bot_ptr];
		*alias = map[id] = new_alias;
		free_list_bot_ptr++;
	}
}

void RegisterFile::free_alias(const reg_t& id) {
	free_list[free_list_top_ptr] = id;
	free_list_top_ptr++;
}


void RegisterFile::read(bool read, const reg_t& id, word_t* read_value) {
	if (read)
		*read_value = registers[map[id]];
}


void RegisterFile::write(bool write, const reg_t& id, word_t write_value) {
	if (write)
		registers[map[id]] = write_value;
}