#include "debug/debug_synthesis.hpp"

#include <cstdint>
#include <functional>
#include <ios>
#include <iostream>
#include <string>

#include "debug/debug.hpp"

const uint16_t physical_register_count      = 64;
const uint16_t architectural_register_count = 32;
const uint16_t reorder_buffer_count         = 16;
const uint16_t issue_table_entries_count    = reorder_buffer_count;

inline int pack(int index, int pack_size) {
	return index / pack_size;
}

inline int get_value(int value, int index, int pack_size, int bits) {
	return (value & (((1 << bits) - 1) << (8 * (index % pack_size)))) >> (8 * (index % pack_size));
}

json dump_register_map(XProcessor* instance) {
	json map = json::object();

	int v;
	for (uint16_t i = 0; i < architectural_register_count; i++) {
		XProcessor_Read_dbg_decode_status_register_map_V_Words(instance, pack(i, 4), &v, 1);
		map.push_back({ std::to_string(i), std::to_string(get_value(v, i, 4, 6)) });
	}

	return map;
}

json dump_ready_registers(XProcessor* instance) {
	json arr = json::array();

	u64 ready_registers = XProcessor_Get_dbg_decode_status_ready_registers_V(instance);
	for (uint16_t i = 0; i < physical_register_count; i++) {
		if (ready_registers & (1 << i))
			arr.push_back(std::to_string(i));
	}

	return arr;
}

json dump_free_aliases(XProcessor* instance) {
	json arr = json::array();

	u32 bot = XProcessor_Get_dbg_decode_status_free_aliases_bot_V(instance);
	u32 top = XProcessor_Get_dbg_decode_status_free_aliases_top_V(instance);
	bool empty = static_cast<bool>(XProcessor_Get_dbg_decode_status_free_aliases_empty_V(instance));

	int v;
	for (uint16_t i = bot, do_loop = !empty ; i != top || do_loop ; i = (i + 1) % physical_register_count, do_loop = false) {
		XProcessor_Read_dbg_decode_status_free_aliases_V_Words(instance, pack(i, 4), &v, 1);
		arr.push_back(get_value(v, i, 4, 6));
	}

	return arr;
}

json dump_register_file(XProcessor* instance) {
	json map = json::object();

	int v;
	for (uint16_t i = 0; i < physical_register_count; i++) {
		XProcessor_Read_dbg_issue_status_register_file_V_Words(instance, i, &v, 1);
		map.push_back({ std::to_string(i), v });
	}

	return map;
}

json dump_issue_table(XProcessor* instance) {
	json obj = json::object();

	int used, pc, func3, is_func7_0b0000000, is_func7_0b0000001, is_func7_0b0100000,
	packed_immediate, use_src1, use_src2, src1, src2, src1_ready, src2_ready;

	for (uint16_t i = 0; i < issue_table_entries_count; i++) {
		XProcessor_Read_dbg_issue_status_issue_table_used_V_Words(instance, pack(i, 4), &used, 1);
		XProcessor_Read_dbg_issue_status_issue_table_pc_V_Words(instance, pack(i, 1), &pc, 1);
		XProcessor_Read_dbg_issue_status_issue_table_func3_V_Words(instance, pack(i, 4), &func3, 1);
		XProcessor_Read_dbg_issue_status_issue_table_is_func7_0b0000000_V_Words(instance, pack(i, 4), &is_func7_0b0000000, 1);
		XProcessor_Read_dbg_issue_status_issue_table_is_func7_0b0000001_V_Words(instance, pack(i, 4), &is_func7_0b0000001, 1);
		XProcessor_Read_dbg_issue_status_issue_table_is_func7_0b0100000_V_Words(instance, pack(i, 4), &is_func7_0b0100000, 1);
		XProcessor_Read_dbg_issue_status_issue_table_packed_immediate_V_Words(instance, pack(i, 1), &packed_immediate, 1);
		XProcessor_Read_dbg_issue_status_issue_table_use_src1_V_Words(instance, pack(i, 4), &use_src1, 1);
		XProcessor_Read_dbg_issue_status_issue_table_use_src2_V_Words(instance, pack(i, 4), &use_src2, 1);
		XProcessor_Read_dbg_issue_status_issue_table_src1_V_Words(instance, pack(i, 4), &src1, 1);
		XProcessor_Read_dbg_issue_status_issue_table_src2_V_Words(instance, pack(i, 4), &src2, 1);
		XProcessor_Read_dbg_issue_status_issue_table_src1_ready_V_Words(instance, pack(i, 4), &src1_ready, 1);
		XProcessor_Read_dbg_issue_status_issue_table_src2_ready_V_Words(instance, pack(i, 4), &src2_ready, 1);

		obj.push_back({ std::to_string(i), {
			{ "used",               static_cast<bool>(get_value(used, i, 4, 1)) },
			{ "pc",                 get_value(pc, i, 1, 30) },
			{ "func3",              get_value(func3, i, 4, 3) },
			{ "is_func7_0b0000000", static_cast<bool>(get_value(is_func7_0b0000000, i, 4, 1)) },
			{ "is_func7_0b0000001", static_cast<bool>(get_value(is_func7_0b0000001, i, 4, 1)) },
			{ "is_func7_0b0100000", static_cast<bool>(get_value(is_func7_0b0100000, i, 4, 1)) },
			{ "packed_immediate",   get_value(packed_immediate, i, 1, 20) },
			{ "use_src1",           static_cast<bool>(get_value(use_src1, i, 4, 1)) },
			{ "use_src2",           static_cast<bool>(get_value(use_src2, i, 4, 1)) },
			{ "src1",               get_value(src1, i, 4, 6) },
			{ "src2",               get_value(src2, i, 4, 6) },
			{ "src1_ready",         static_cast<bool>(get_value(src1_ready, i, 4, 1)) },
			{ "src2_ready",         static_cast<bool>(get_value(src2_ready, i, 4, 1)) }
		} });
	}

	return obj;
}

json dump_free_entries(XProcessor* instance) {
	json arr = json::array();

	u32 bot = XProcessor_Get_dbg_issue_status_free_entries_bot_V(instance);
	u32 top = XProcessor_Get_dbg_issue_status_free_entries_top_V(instance);
	bool empty = static_cast<bool>(XProcessor_Get_dbg_issue_status_free_entries_empty_V(instance));

	int v;
	for (uint16_t i = bot, do_loop = !empty ; i != top || do_loop ; i = (i + 1) % issue_table_entries_count, do_loop = false) {
		XProcessor_Read_dbg_issue_status_free_entries_V_Words(instance, pack(i, 4), &v, 1);
		arr.push_back(get_value(v, i, 4, 4));
	}

	return arr;
}

json dump_reorder_buffer(XProcessor* instance) {
	json arr = json::array();

	u32 bot = XProcessor_Get_dbg_commit_status_reorder_buffer_bot_V(instance);
	u32 top = XProcessor_Get_dbg_commit_status_reorder_buffer_top_V(instance);
	bool empty = static_cast<bool>(XProcessor_Get_dbg_commit_status_reorder_buffer_empty_V(instance));

	int token, done, invalid;
	for (uint16_t i = bot, do_loop = !empty ; i != top || do_loop ; i = (i + 1) % reorder_buffer_count, do_loop = false) {
		XProcessor_Read_dbg_commit_status_reorder_buffer_token_V_Words(instance, pack(i, 4), &token, 1);
		XProcessor_Read_dbg_commit_status_reorder_buffer_done_V_Words(instance, pack(i, 4), &done, 1);
		XProcessor_Read_dbg_commit_status_reorder_buffer_invalid_V_Words(instance, pack(i, 4), &invalid, 1);
		arr.push_back({
			{ "token",   get_value(token, i, 4, 4) },
			{ "done",    static_cast<bool>(get_value(done, i, 4, 4)) },
			{ "invalid", static_cast<bool>(get_value(invalid, i, 4, 4)) },
		});
	}

	return arr;
}

json dump_debug_info(XProcessor* instance) {
	return {
		{ "cycle", XProcessor_Get_dbg_cycle_V(instance) },
		{ "fetch status", {
			{ "program_counter", string_hex(XProcessor_Get_dbg_fetch_status_program_counter_V(instance)) },
			{ "did_smth",        static_cast<bool>(XProcessor_Get_dbg_fetch_status_did_smth_V(instance)) }
		} },
		{ "decode status", {
			{ "register_map",       dump_register_map(instance) },
			{ "ready_registers",    dump_ready_registers(instance) },
			{ "free_aliases",       dump_free_aliases(instance) },
			{ "free_aliases_bot",   XProcessor_Get_dbg_decode_status_free_aliases_bot_V(instance) },
			{ "free_aliases_top",   XProcessor_Get_dbg_decode_status_free_aliases_top_V(instance) },
			{ "free_aliases_empty", static_cast<bool>(XProcessor_Get_dbg_decode_status_free_aliases_empty_V(instance)) },
			{ "free_aliases_full",  static_cast<bool>(XProcessor_Get_dbg_decode_status_free_aliases_full_V(instance)) },
			{ "did_smth",           static_cast<bool>(XProcessor_Get_dbg_decode_status_did_smth_V(instance)) }
		} },
		{ "issue status", {
			{ "register_file",      dump_register_file(instance) },
			{ "issue_table",        dump_issue_table(instance) },
			{ "free_entries",       dump_free_entries(instance) },
			{ "free_entries_bot",   XProcessor_Get_dbg_issue_status_free_entries_bot_V(instance) },
			{ "free_entries_top",   XProcessor_Get_dbg_issue_status_free_entries_top_V(instance) },
			{ "free_entries_empty", static_cast<bool>(XProcessor_Get_dbg_issue_status_free_entries_empty_V(instance)) },
			{ "free_entries_full",  static_cast<bool>(XProcessor_Get_dbg_issue_status_free_entries_full_V(instance)) }
		} },
		{ "write back status", {

		} },
		{ "commit status", {
			{ "reorder_buffer",       dump_reorder_buffer(instance) },
			{ "reorder_buffer_bot",   XProcessor_Get_dbg_commit_status_reorder_buffer_bot_V(instance) },
			{ "reorder_buffer_top",   XProcessor_Get_dbg_commit_status_reorder_buffer_top_V(instance) },
			{ "reorder_buffer_empty", static_cast<bool>(XProcessor_Get_dbg_commit_status_reorder_buffer_empty_V(instance)) },
			{ "reorder_buffer_full",  static_cast<bool>(XProcessor_Get_dbg_commit_status_reorder_buffer_full_V(instance)) }
		} },
		{ "fetch to decode", {
			{ "has_fetched", static_cast<bool>(XProcessor_Get_dbg_fetch_to_decode_has_fetched_V(instance)) },
			{ "instruction", string_hex(XProcessor_Get_dbg_fetch_to_decode_instruction_V(instance)) },
			{ "pc",          string_hex(XProcessor_Get_dbg_fetch_to_decode_pc_V(instance)) }
		} },
		{ "decode to fetch", {
			{ "has_next_pc", static_cast<bool>(XProcessor_Get_dbg_decode_to_fetch_has_next_pc_V(instance))},
			{ "next_pc",     string_hex(XProcessor_Get_dbg_decode_to_fetch_next_pc_V(instance))},
		} },
		{ "decode to issue", {
			{ "has_decoded_instruction", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_has_decoded_instruction_V(instance)) },
			{ "pc",                      string_hex(XProcessor_Get_dbg_decode_to_issue_pc_V(instance)) },
			{ "func3",                   string_bin<3>(XProcessor_Get_dbg_decode_to_issue_func3_V(instance)) },
			{ "is_func7_0b0000000",      static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0000000_V(instance)) },
			{ "is_func7_0b0000001",      static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0000001_V(instance)) },
			{ "is_func7_0b0100000",      static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0100000_V(instance)) },
			{ "packed_immediate",        string_hex(XProcessor_Get_dbg_decode_to_issue_packed_immediate_V(instance)) },
			{ "use_dest",                static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_dest_V(instance)) },
			{ "use_src1",                static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_src1_V(instance)) },
			{ "use_src2",                static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_src2_V(instance)) },
			{ "dest",                    XProcessor_Get_dbg_decode_to_issue_dest_V(instance) },
			{ "src1",                    XProcessor_Get_dbg_decode_to_issue_src1_V(instance) },
			{ "src2",                    XProcessor_Get_dbg_decode_to_issue_src2_V(instance) },
			{ "src1_ready",              static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_src1_ready_V(instance)) },
			{ "src2_ready",              static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_src2_ready_V(instance)) },
			{ "invalid",                 static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_invalid_V(instance)) }
		} },
		{ "decode to commit", {
			{ "add_to_rob", static_cast<bool>(XProcessor_Get_dbg_decode_to_commit_add_to_rob_V(instance)) },
			{ "token",      XProcessor_Get_dbg_decode_to_commit_token_V(instance) },
			{ "invalid",    static_cast<bool>(XProcessor_Get_dbg_decode_to_commit_invalid_V(instance)) }
		} },
		{ "commit to commit", {
			{ "rob_was_empty",        static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_rob_was_empty_V(instance)) },
			{ "previous_first_entry", {
				{ "token",   XProcessor_Get_dbg_commit_to_commit_previous_first_entry_token_V(instance) },
				{ "done",    static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_previous_first_entry_done_V(instance)) },
				{ "invalid", static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_previous_first_entry_invalid_V(instance)) }
			} }
		} }
	};
}
