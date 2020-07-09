#include "components/register_map.hpp"

#include "common.hpp"
#include "debug/debug.hpp"
#include "architecture/architecture_helpers.hpp"
#include "debug/debug_helpers.hpp"

#include <vector>

RegisterMap::RegisterMap() {
	for (uint16_t id = 0; id < architectural_register_count; id++) {
		#pragma HLS UNROLL
		map[id] = id;
	}
	for (uint16_t id = architectural_register_count; id < physical_register_count; id++) {
		#pragma HLS UNROLL
		free_list.push_back(id);
	}
}

void RegisterMap::get_alias(const reg_t& id, physical_reg_t* alias) {
	*alias = map[id];
}

void RegisterMap::create_alias(const reg_t& id, physical_reg_t* alias, bool* blocking) {
	if (free_list.is_empty()) {
		*blocking = true;
	}
	else {
		physical_reg_t new_alias = free_list.pop();
		*alias = map[id] = new_alias;
	}

	#ifndef __SYNTHESIS__
	json json_map = json::object();
	for (uint16_t id = 0 ; id < architectural_register_count ; id++)
		json_map.push_back({ to_string(static_cast<reg_t>(id)), map[id].to_uint() });
	Debugger::add_cycle_event({
		{ "Creating alias",
			{
				{ "Architectural register", to_string(id) },
				{ "Physical register created", alias->to_uint() },
				{ "Current mapping", json_map },
				{ "Free registers", free_list }
			}
		}
	});
	#endif // __SYNTHESIS__
}

void RegisterMap::free_alias(const physical_reg_t& id) {
	free_list.push_back(id);

	#ifndef __SYNTHESIS__
	Debugger::add_cycle_event({
		{ "Freeing alias",
			{
				{ "Architectural register", to_string(id) },
				{ "Free registers", free_list }
			}
		}
	});
	#endif // __SYNTHESIS__
}
