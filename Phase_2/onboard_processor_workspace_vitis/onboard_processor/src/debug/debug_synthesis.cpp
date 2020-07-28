#include "debug/debug_synthesis.hpp"

#include <cstdint>
#include <functional>
#include <ios>
#include <iostream>
#include <string>

#include "debug/debug_helpers.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

static struct indent_t {
    std::string i = "";
    inline std::string add() { i.push_back('\t'); return i; }
    inline std::string sub() { i.pop_back(); return i; }
    inline friend std::ostream& operator<<(std::ostream& s, indent_t& indent) {
        s << indent.i;
        return s;
    }
} indent;

template<int count>
struct json_array {
    using F = std::function<void(std::ostream&, int)>;
    F f;
    json_array(F f_) : f(f_) {}
    inline friend std::ostream& operator<<(std::ostream& s, json_array<count>&& arr) {
        s << std::forward<json_array<count>&>(arr);
        return s;
    }
    inline friend std::ostream& operator<<(std::ostream& s, json_array<count>& arr) {
        s << "[ ";
        for (int i = 0; i < count; i++) {
            arr.f(s, i);
			s << (i + 1 != count ? ", " : "");
			s.flush();
		}
		s << " ]";
        return s;
    }
};

template<int count>
struct json_circular_array {
    using F = std::function<void(std::ostream&, int)>;
    int bot; int top; bool empty; F f;
    json_circular_array(int bot_, int top_, bool empty_, F f_) : bot(bot_), top(top_), empty(empty_), f(f_) {}
    inline friend std::ostream& operator<<(std::ostream& s, json_circular_array<count>&& arr) {
        s << std::forward<json_circular_array<count>&>(arr);
        return s;
    }
    inline friend std::ostream& operator<<(std::ostream& s, json_circular_array<count>& arr) {
        s << "[ ";
        for (int i = arr.bot, do_loop = !arr.empty; i != arr.top || do_loop; i = (i + 1) % count, do_loop = false)  {
            arr.f(s, i);
            s << ((i + 1) % count != arr.top ? ", " : "");
			s.flush();
        }
        s << " ]";
        return s;
    }
};

template<int count>
struct json_list {
    using F = std::function<void(std::ostream&, int)>;
    using P = std::function<bool(int)>;
    F f; P p;
    json_list(F f_, P p_) :f(f_), p(p_) {}
    inline friend std::ostream& operator<<(std::ostream& s, json_list<count>&& list) {
        s << std::forward<json_list<count>&>(list);
        return s;
    }
    inline friend std::ostream& operator<<(std::ostream& s, json_list<count>& list) {
        s << "[ ";
        for (int i = 0; i < count; i++) {
            if (list.p(i)) {
                list.f(s, i);
                s << (i + 1 != count ? ", " : "");
            }
        }
        s << " ]";
        return s;
    }
};

template<typename T>
struct json_pair {
    std::string string; T t;
    json_pair(std::string string_, T t_) : string(string_), t(t_) {}
    inline friend std::ostream& operator<<(std::ostream& s, json_pair<T>&& pair) {
        s << std::forward<json_pair<T>&>(pair);
        return s;
    }
    inline friend std::ostream& operator<<(std::ostream& s, json_pair<T>& pair) {
        s << '"' << pair.string << '"' << " : " << pair.t;
        return s;
    }
};

template<typename T, typename... U>
struct json_object {
    T t;
    json_object<U...> obj;
    json_object(T t_, U... u) : t(t_), obj(u...) {}
    inline friend std::ostream& operator<<(std::ostream& s, json_object<T, U...>&& obj) {
        s << std::forward<json_object<T, U...>&>(obj);
        return s;
    }
    inline friend std::ostream& operator<<(std::ostream& s, json_object<T, U...>& obj) {
        s << obj.t << ",\n" << indent << obj.obj;
        return s;
    }
};

template<typename T>
struct json_object<T> {
    T t;
    json_object(T t_) : t(t_) {}
    inline friend std::ostream& operator<<(std::ostream& s, json_object<T>&& obj) {
        s << std::forward<json_object<T>&>(obj);
        return s;
    }
    inline friend std::ostream& operator<<(std::ostream& s, json_object<T>& obj) {
        s << obj.t;
        return s;
    }
};

struct json_object_light {
	using F = std::function<void(std::ostream&)>;
    F f;
    json_object_light(F f_) : f(f_) {}
    inline friend std::ostream& operator<<(std::ostream& s, json_object_light&& obj) {
        s << std::forward<json_object_light&>(obj);
        return s;
    }
    inline friend std::ostream& operator<<(std::ostream& s, json_object_light& obj) {
        obj.f(s);
        return s;
    }
};

template<typename... T>
struct json_object_holder {
    json_object<T...> obj;
    json_object_holder(T... t) : obj(t...) {}
    inline friend std::ostream& operator<<(std::ostream& s, json_object_holder<T...>&& obj) {
        s << std::forward<json_object_holder<T...>&>(obj);
        return s;
    }
    inline friend std::ostream& operator<<(std::ostream& s, json_object_holder<T...>& obj) {
        s << "{\n" << indent.add() << obj.obj << '\n' << indent.sub() << '}';
        return s;
    }
};

#define PP_ARG_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
         _61, _62, _63, N, ...) N

#define PP_RSEQ_N()                                        \
         63, 62, 61, 60,                                   \
         59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
         49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
         39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
         19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
          9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define PP_NARG_(...)    PP_ARG_N(__VA_ARGS__)

#define PP_COMMASEQ_N()                                    \
          1,  1,  1,  1,                                   \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,           \
          1,  1,  1,  1,  1,  1,  1,  1,  0,  0

#define PP_COMMA(...)    ,

#define PP_HASCOMMA(...)                                   \
          PP_NARG_(__VA_ARGS__, PP_COMMASEQ_N())

#define PP_NARG(...)                                       \
          PP_NARG_HELPER1(                                 \
              PP_HASCOMMA(__VA_ARGS__),                    \
              PP_HASCOMMA(PP_COMMA __VA_ARGS__ ()),        \
              PP_NARG_(__VA_ARGS__, PP_RSEQ_N()))

#define PP_NARG_HELPER1(a, b, N)    PP_NARG_HELPER2(a, b, N)
#define PP_NARG_HELPER2(a, b, N)    PP_NARG_HELPER3_ ## a ## b(N)
#define PP_NARG_HELPER3_01(N)    0
#define PP_NARG_HELPER3_00(N)    1
#define PP_NARG_HELPER3_11(N)    N

#define PACK(pack_size) i / pack_size
#define GET_VAL(pack_size, bit_count) ((v & (((1 << bit_count) - 1) << (8 * (i % pack_size)))) >> (8 * (i % pack_size)))
#define PAIR_END << ",\n"
#define PAIR(str, obj) json_pair<decltype(obj)>(str, obj)
#define DEDUCE_TYPE_1(first) decltype(first)
#define DEDUCE_TYPE_2(first, others...)  decltype(first), DEDUCE_TYPE_1(others)
#define DEDUCE_TYPE_3(first, others...)  decltype(first), DEDUCE_TYPE_2(others)
#define DEDUCE_TYPE_4(first, others...)  decltype(first), DEDUCE_TYPE_3(others)
#define DEDUCE_TYPE_5(first, others...)  decltype(first), DEDUCE_TYPE_4(others)
#define DEDUCE_TYPE_6(first, others...)  decltype(first), DEDUCE_TYPE_5(others)
#define DEDUCE_TYPE_7(first, others...)  decltype(first), DEDUCE_TYPE_6(others)
#define DEDUCE_TYPE_8(first, others...)  decltype(first), DEDUCE_TYPE_7(others)
#define DEDUCE_TYPE_9(first, others...)  decltype(first), DEDUCE_TYPE_8(others)
#define DEDUCE_TYPE_10(first, others...) decltype(first), DEDUCE_TYPE_9(others)
#define DEDUCE_TYPE_11(first, others...) decltype(first), DEDUCE_TYPE_10(others)
#define DEDUCE_TYPE_12(first, others...) decltype(first), DEDUCE_TYPE_11(others)
#define DEDUCE_TYPE_13(first, others...) decltype(first), DEDUCE_TYPE_12(others)
#define DEDUCE_TYPE_14(first, others...) decltype(first), DEDUCE_TYPE_13(others)
#define DEDUCE_TYPE1(count, others...)   DEDUCE_TYPE2(count, others)
#define DEDUCE_TYPE2(count, others...)   DEDUCE_TYPE_##count(others)
#define OBJECT_START "{\n" << (indent.add(), "")
#define OBJECT_END   << '\n' << indent.sub() << '}'
#define OBJECT(elem...) json_object_holder<DEDUCE_TYPE1(PP_NARG(elem), elem)>(elem)
#define OBJECT_C(func) json_object_light([&](std::ostream& s){ int v; s << OBJECT_START func OBJECT_END; })
#define ARRAY(count, prev, func) json_array<count>([&](std::ostream& s, int i){ int v; prev; s << func; })
#define LIST(count, prev, func, pred) json_list<count>([&](std::ostream& s, int i){ int v; prev; s << func; }, [&](int i) -> bool { return pred; })
#define CIRCULAR_ARRAY(max_count, bot, top, empty, prev, func) json_circular_array<max_count>(bot, top, empty, [&](std::ostream& s, int i){ int v; prev; s << func; })

void dumpDebugInfo(XProcessor* instance) {
	auto fetch_status = OBJECT(
		PAIR("program_counter", XProcessor_Get_dbg_fetch_status_program_counter_V(instance)),
		PAIR("did_smth", static_cast<bool>(XProcessor_Get_dbg_fetch_status_did_smth_V(instance)))
	);

	auto decode_status_register_map = ARRAY(32, XProcessor_Read_dbg_decode_status_register_map_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 6));
	auto decode_status_free_registers = LIST(64, , std::to_string(i), XProcessor_Get_dbg_decode_status_ready_registers_V(instance) & (1 << i));
	auto decode_status_free_aliases = CIRCULAR_ARRAY(64,
		XProcessor_Get_dbg_decode_status_free_aliases_bot_V(instance),
		XProcessor_Get_dbg_decode_status_free_aliases_top_V(instance),
		static_cast<bool>(XProcessor_Get_dbg_decode_status_free_aliases_empty_V(instance)),
		XProcessor_Read_dbg_decode_status_free_aliases_V_Words(instance, PACK(4), &v, 1),
		GET_VAL(4, 6)
	);

	auto decode_status = "[ \"not dumped\" ]";
	// OBJECT(
	// 	PAIR("register_map", decode_status_register_map),
	// 	PAIR("ready_registers", decode_status_free_registers),
	// 	PAIR("free_aliases", decode_status_free_aliases),
	// 	PAIR("free_aliases_bot", XProcessor_Get_dbg_decode_status_free_aliases_bot_V(instance)),
	// 	PAIR("free_aliases_top", XProcessor_Get_dbg_decode_status_free_aliases_top_V(instance)),
	// 	PAIR("free_aliases_empty", static_cast<bool>(XProcessor_Get_dbg_decode_status_free_aliases_empty_V(instance))),
	// 	PAIR("free_aliases_full", static_cast<bool>(XProcessor_Get_dbg_decode_status_free_aliases_full_V(instance))),
	// 	PAIR("did_smth", static_cast<bool>(XProcessor_Get_dbg_decode_status_did_smth_V(instance)))
	// );

	auto issue_status_register_file = ARRAY(64, XProcessor_Read_dbg_issue_status_register_file_V_Words(instance, PACK(1), &v, 1), v);
	auto issue_status_issue_table = "[ \"not dumped\" ]";
	//ARRAY(16, , OBJECT_C(
	// 	<< indent << PAIR("used", static_cast<bool>(XProcessor_Read_dbg_issue_status_issue_table_used_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1))) PAIR_END
	// 	<< indent << PAIR("pc", (XProcessor_Read_dbg_issue_status_issue_table_pc_V_Words(instance, PACK(1), &v, 1), GET_VAL(1, 30))) PAIR_END
	// 	<< indent << PAIR("func3", (XProcessor_Read_dbg_issue_status_issue_table_func3_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 3))) PAIR_END
	// 	<< indent << PAIR("is_func7_0b0000000", static_cast<bool>(XProcessor_Read_dbg_issue_status_issue_table_is_func7_0b0000000_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1))) PAIR_END
	// 	<< indent << PAIR("is_func7_0b0000001", static_cast<bool>(XProcessor_Read_dbg_issue_status_issue_table_is_func7_0b0000001_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1))) PAIR_END
	// 	<< indent << PAIR("is_func7_0b0100000", static_cast<bool>(XProcessor_Read_dbg_issue_status_issue_table_is_func7_0b0100000_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1))) PAIR_END
	// 	<< indent << PAIR("packed_immediate", (XProcessor_Read_dbg_issue_status_issue_table_packed_immediate_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 20))) PAIR_END
	// 	<< indent << PAIR("use_src1", static_cast<bool>(XProcessor_Read_dbg_issue_status_issue_table_use_src1_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1))) PAIR_END
	// 	<< indent << PAIR("use_src2", static_cast<bool>(XProcessor_Read_dbg_issue_status_issue_table_use_src2_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1))) PAIR_END
	// 	<< indent << PAIR("src1", (XProcessor_Read_dbg_issue_status_issue_table_src1_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 6))) PAIR_END
	// 	<< indent << PAIR("src2", (XProcessor_Read_dbg_issue_status_issue_table_src2_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 6))) PAIR_END
	// 	<< indent << PAIR("src1_ready", static_cast<bool>(XProcessor_Read_dbg_issue_status_issue_table_src1_ready_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1))) PAIR_END
	// 	<< indent << PAIR("src2_ready", static_cast<bool>(XProcessor_Read_dbg_issue_status_issue_table_src2_ready_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1)))
	// ));
	auto issue_status_free_entries = ARRAY(16, XProcessor_Read_dbg_issue_status_free_entries_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 4));

	auto issue_status = "[ \"not dumped\" ]";
	// OBJECT(
	// 	PAIR("register_file", issue_status_register_file),
	// 	PAIR("issue_table", issue_status_issue_table),
	// 	PAIR("free_entries", issue_status_free_entries),
	// 	PAIR("free_entries_bot", XProcessor_Get_dbg_issue_status_free_entries_bot_V(instance)),
	// 	PAIR("free_entries_top", XProcessor_Get_dbg_issue_status_free_entries_top_V(instance)),
	// 	PAIR("free_entries_empty", static_cast<bool>(XProcessor_Get_dbg_issue_status_free_entries_empty_V(instance))),
	// 	PAIR("free_entries_full", static_cast<bool>(XProcessor_Get_dbg_issue_status_free_entries_full_V(instance)))
	// );

	auto commit_status_rob = "[ \"not dumped\" ]";
	// ARRAY(16, , OBJECT_C(
	// 	<< indent << PAIR("token", (XProcessor_Read_dbg_commit_status_reorder_buffer_token_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 4))) PAIR_END
	// 	<< indent << PAIR("done", static_cast<bool>(XProcessor_Read_dbg_commit_status_reorder_buffer_done_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1))) PAIR_END
	// 	<< indent << PAIR("invalid", static_cast<bool>(XProcessor_Read_dbg_commit_status_reorder_buffer_invalid_V_Words(instance, PACK(4), &v, 1), GET_VAL(4, 1)))
	// ));

	auto commit_status = "[ \"not dumped\" ]";
	// OBJECT(
	// 	PAIR("reorder_buffer", commit_status_rob),
	// 	PAIR("reorder_buffer_bot", XProcessor_Get_dbg_commit_status_reorder_buffer_bot_V(instance)),
	// 	PAIR("reorder_buffer_top", XProcessor_Get_dbg_commit_status_reorder_buffer_top_V(instance)),
	// 	PAIR("reorder_buffer_empty", static_cast<bool>(XProcessor_Get_dbg_commit_status_reorder_buffer_empty_V(instance))),
	// 	PAIR("reorder_buffer_full", static_cast<bool>(XProcessor_Get_dbg_commit_status_reorder_buffer_full_V(instance)))
	// );

	auto decode_to_issue = "[ \"not dumped\" ]";
	// OBJECT_C(
	// 	<< indent << PAIR("has_decoded_instruction", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_has_decoded_instruction_V(instance))) PAIR_END
	// 	<< indent << PAIR("pc", XProcessor_Get_dbg_decode_to_issue_pc_V(instance)) PAIR_END
	// 	<< indent << PAIR("func3", XProcessor_Get_dbg_decode_to_issue_func3_V(instance)) PAIR_END
	// 	<< indent << PAIR("is_func7_0b0000000", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0000000_V(instance))) PAIR_END
	// 	<< indent << PAIR("is_func7_0b0000001", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0000001_V(instance))) PAIR_END
	// 	<< indent << PAIR("is_func7_0b0100000", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_is_func7_0b0100000_V(instance))) PAIR_END
	// 	<< indent << PAIR("packed_immediate", XProcessor_Get_dbg_decode_to_issue_packed_immediate_V(instance)) PAIR_END
	// 	<< indent << PAIR("use_dest", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_dest_V(instance))) PAIR_END
	// 	<< indent << PAIR("use_src1", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_src1_V(instance))) PAIR_END
	// 	<< indent << PAIR("use_src2", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_use_src2_V(instance))) PAIR_END
	// 	<< indent << PAIR("dest", XProcessor_Get_dbg_decode_to_issue_dest_V(instance)) PAIR_END
	// 	<< indent << PAIR("src1", XProcessor_Get_dbg_decode_to_issue_src1_V(instance)) PAIR_END
	// 	<< indent << PAIR("src2", XProcessor_Get_dbg_decode_to_issue_src2_V(instance)) PAIR_END
	// 	<< indent << PAIR("invalid_instruction", static_cast<bool>(XProcessor_Get_dbg_decode_to_issue_invalid_V(instance)))
	// );

	std::cout << std::boolalpha << OBJECT_C(
		<< indent << PAIR("cycle", XProcessor_Get_dbg_cycle_V(instance)) PAIR_END
		<< indent << PAIR("fetch_status", fetch_status) PAIR_END
		<< indent << PAIR("decode_status", decode_status) PAIR_END
		<< indent << PAIR("issue_status", issue_status) PAIR_END
		<< indent << PAIR("commit_status", commit_status) PAIR_END
		<< indent << PAIR("fetch_to_decode", OBJECT(
			PAIR("has_fetched", static_cast<bool>(XProcessor_Get_dbg_fetch_to_decode_has_fetched_V(instance))),
			PAIR("instruction", string_hex(XProcessor_Get_dbg_fetch_to_decode_instruction_V(instance))),
			PAIR("pc", string_hex(XProcessor_Get_dbg_fetch_to_decode_pc_V(instance)))
		)) PAIR_END
		<< indent << PAIR("decode_to_fetch", OBJECT(
			PAIR("has_next_pc", static_cast<bool>(XProcessor_Get_dbg_decode_to_fetch_has_next_pc_V(instance))),
			PAIR("next_pc", string_hex(XProcessor_Get_dbg_decode_to_fetch_next_pc_V(instance)))
		)) PAIR_END
		<< indent << PAIR("decode_to_issue", decode_to_issue) PAIR_END
		<< indent << PAIR("decode_to_commit", OBJECT(
			PAIR("add_to_rob", static_cast<bool>(XProcessor_Get_dbg_decode_to_commit_add_to_rob_V(instance))),
			PAIR("token", string_hex(XProcessor_Get_dbg_decode_to_commit_token_V(instance))),
			PAIR("invalid", static_cast<bool>(XProcessor_Get_dbg_decode_to_commit_invalid_V(instance)))
		)) PAIR_END
		<< indent << PAIR("commit_to_commit", OBJECT(
			PAIR("has_next_pc", static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_rob_was_empty_V(instance))),
			PAIR("first_entry_token", string_hex(XProcessor_Get_dbg_commit_to_commit_previous_first_entry_token_V(instance))),
			PAIR("first_entry_done", static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_previous_first_entry_done_V(instance))),
			PAIR("first_entry_invalid", static_cast<bool>(XProcessor_Get_dbg_commit_to_commit_previous_first_entry_invalid_V(instance)))
		))
	);
}

#pragma GCC diagnostic pop
