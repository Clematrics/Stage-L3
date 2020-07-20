#include "debug/debug_synthesis.hpp"

#include <cstdint>
#include <iostream>
#include <string>

#include "debug/debug_helpers.hpp"

void dumpDebugInfo(XProcessor* instance) {
	uint32_t cycle = XProcessor_Get_dbg_cycle_V(instance);
	bool has_fetched = static_cast<bool>(XProcessor_Get_dbg_fetch_to_decode_has_fetched(instance));
	std::string instruction = string_hex(XProcessor_Get_dbg_fetch_to_decode_instruction_V(instance));
	std::string pc = string_hex(XProcessor_Get_dbg_fetch_to_decode_pc_V(instance));
	bool has_next_pc = static_cast<bool>(XProcessor_Get_dbg_decode_to_fetch_has_next_pc(instance));
	std::string next_pc = string_hex(XProcessor_Get_dbg_decode_to_fetch_next_pc_V(instance));
	bool fetch_locked = static_cast<bool>(XProcessor_Get_dbg_fetch_locked(instance));
	bool decode_locked = static_cast<bool>(XProcessor_Get_dbg_decode_locked(instance));

	std::cout <<
		"{ { \"cycle\", " << cycle << " }, " <<
		"{ \"fetch_to_decode\", " <<
			"{ \"has_fetched\", " << has_fetched << " }, " <<
			"{ \"instruction\", " << instruction << " }, " <<
			"{ \"pc\", " << pc << " }" <<
		" }, " <<
		"{ \"decode_to_fetch\", " <<
			"{ \"has_next_pc\", " << has_next_pc << " }, " <<
			"{ \"next_pc\", " << next_pc << " }" <<
		" }, " <<
		"{ \"fetch_locked\", " << fetch_locked << " }, " <<
		"{ \"decode_locked\", " << decode_locked << " }" <<
		" }\n";

	// return
	// {
	// 	{ "Cycle", XProcessor_Get_dbg_cycle_V(instance) },
	// 	{ "fetch_to_decode",
	// 		{
	// 			{ "has_fetched", static_cast<bool>(XProcessor_Get_dbg_fetch_to_decode_has_fetched(instance)) },
	// 			{ "instruction", string_hex(XProcessor_Get_dbg_fetch_to_decode_instruction_V(instance)) },
	// 			{ "pc", string_hex(XProcessor_Get_dbg_fetch_to_decode_pc_V(instance)) },
	// 		}
	// 	},
	// 	{ "decode_to_fetch",
	// 		{
	// 			{ "has_next_pc", static_cast<bool>(XProcessor_Get_dbg_decode_to_fetch_has_next_pc(instance)) },
	// 			{ "next_pc", string_hex(XProcessor_Get_dbg_decode_to_fetch_next_pc_V(instance)) }
	// 		}
	// 	}
	// };
}