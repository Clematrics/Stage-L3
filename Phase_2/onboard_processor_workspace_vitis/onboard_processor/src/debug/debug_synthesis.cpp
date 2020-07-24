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
#define OBJECT10(indent, pair, pairs...) indent << pair << ",\n" << OBJECT9(indent, pairs)
#define OBJECT11(indent, pair, pairs...) indent << pair << ",\n" << OBJECT10(indent, pairs)
#define OBJECT12(indent, pair, pairs...) indent << pair << ",\n" << OBJECT11(indent, pairs)
#define OBJECT13(indent, pair, pairs...) indent << pair << ",\n" << OBJECT12(indent, pairs)
#define OBJECT14(indent, pair, pairs...) indent << pair << ",\n" << OBJECT13(indent, pairs)

#define OBJECT(count, indent, content...) "{\n" << OBJECT##count(indent "  ", content) << "\n" << indent << "}"


void dumpDebugInfo(XProcessor* instance) {
	std::cout <<
	OBJECT(8, "",
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
		PAIR("decode_to_issue", OBJECT(14, "  ",
			PAIR("has_decoded_instruction", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_has_decoded_instruction_V(instance))),
			PAIR("pc", XProcessor_Get_dbg_decode_to_issue_pc_V(instance)),
			PAIR("func3", XProcessor_Get_dbg_decode_to_issue_func3_V(instance)),
			PAIR("is_func7_0b0000000", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0000000_V(instance))),
			PAIR("is_func7_0b0000001", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0000001_V(instance))),
			PAIR("is_func7_0b0100000", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0100000_V(instance))),
			PAIR("packed_immediate", XProcessor_Get_dbg_decode_to_issue_packed_immediate_V(instance)),
			PAIR("use_dest", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_dest_V(instance))),
			PAIR("use_src1", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_src1_V(instance))),
			PAIR("use_src2", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_src2_V(instance))),
			PAIR("dest", XProcessor_Get_dbg_decode_to_issue_dest_V(instance)),
			PAIR("src1", XProcessor_Get_dbg_decode_to_issue_src1_V(instance)),
			PAIR("src2", XProcessor_Get_dbg_decode_to_issue_src2_V(instance)),
			PAIR("invalid_instruction", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_invalid_V(instance)))
		)),
		PAIR("decode_to_commit", OBJECT(3, "  ",
			PAIR("add_to_rob", static_cast<bool>(XProcessor_Get_dbg_decode_to_commit_add_to_rob_V(instance))),
			PAIR("token", string_hex(XProcessor_Get_dbg_decode_to_commit_token_V(instance))),
			PAIR("invalid", static_cast<bool>(XProcessor_Get_dbg_decode_to_commit_invalid_V(instance)))
		)),
		PAIR("commit_to_commit", OBJECT(4, "  ",
			PAIR("has_next_pc", static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_rob_was_empty_V(instance))),
			PAIR("first_entry_token", string_hex(XProcessor_Get_dbg_commit_to_commit_previous_first_entry_token_V(instance))),
			PAIR("first_entry_done", static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_previous_first_entry_done_V(instance))),
			PAIR("first_entry_invalid", static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_previous_first_entry_invalid_V(instance)))
		)),
		PAIR("fetch_ran", static_cast<bool>(XProcessor_Get_dbg_fetch_ran_V(instance))),
		PAIR("decode_ran", static_cast<bool>(XProcessor_Get_dbg_decode_ran_V(instance)))
	) << '\n';
}
