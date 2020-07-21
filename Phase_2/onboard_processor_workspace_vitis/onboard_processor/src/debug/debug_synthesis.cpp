#include "debug/debug_synthesis.hpp"

#include <cstdint>
#include <iostream>
#include <string>

#include "debug/debug_helpers.hpp"

#define PAIR(str, value) "{ " << str << " : " << value << " }"
#define OBJECT1(indent, pair) indent << pair
#define OBJECT2(indent, pair, pairs...) indent << pair << ",\n" << OBJECT1(indent, pairs)
#define OBJECT3(indent, pair, pairs...) indent << pair << ",\n" << OBJECT2(indent, pairs)
#define OBJECT4(indent, pair, pairs...) indent << pair << ",\n" << OBJECT3(indent, pairs)
#define OBJECT5(indent, pair, pairs...) indent << pair << ",\n" << OBJECT4(indent, pairs)
#define OBJECT6(indent, pair, pairs...) indent << pair << ",\n" << OBJECT5(indent, pairs)
#define OBJECT7(indent, pair, pairs...) indent << pair << ",\n" << OBJECT6(indent, pairs)
#define OBJECT8(indent, pair, pairs...) indent << pair << ",\n" << OBJECT7(indent, pairs)
#define OBJECT9(indent, pair, pairs...) indent << pair << ",\n" << OBJECT8(indent, pairs)

#define OBJECT(count, indent, content...) "{\n" << OBJECT##count(indent "  ", content) << "\n" << indent << "}"


void dumpDebugInfo(XProcessor* instance) {
	std::cout <<
	OBJECT(5, "",
		PAIR("cycle", XProcessor_Get_dbg_cycle_V(instance)),
		PAIR("fetch_to_decode", OBJECT(3, "  ",
			PAIR("has_fetched", static_cast<bool>(XProcessor_Get_dbg_fetch_to_decode_has_fetched_V(instance))),
			PAIR("instruction", string_hex(XProcessor_Get_dbg_fetch_to_decode_instruction_V(instance))),
			PAIR("pc", string_hex(XProcessor_Get_dbg_fetch_to_decode_pc_V(instance)))
		)),
		PAIR("decode_to_fetch", OBJECT(2, "  ",
			PAIR("has_next_pc", static_cast<bool>(XProcessor_Get_dbg_decode_to_fetch_has_next_pc_V(instance))),
			PAIR("next_pc", string_hex(XProcessor_Get_dbg_decode_to_fetch_next_pc_V(instance)))
		)),
		PAIR("fetch_ran", static_cast<bool>(XProcessor_Get_dbg_fetch_ran_V(instance))),
		PAIR("decode_ran", static_cast<bool>(XProcessor_Get_dbg_decode_ran_V(instance)))
	) << '\n';
}
