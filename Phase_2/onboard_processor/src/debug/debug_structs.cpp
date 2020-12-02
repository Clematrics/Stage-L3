/* ****************************************************************************
*    Source file defining all functions to convert stage structs to json
**************************************************************************** */

#ifndef __SYNTHESIS__

#include "debug/debug_structs.hpp"

/* ****************************************************************************
*    For intra-stage structures
**************************************************************************** */

json to_json(const IssueTableEntry& entry) {
	return {
		{ "used",               entry.used.to_bool()               },
		{ "token",              entry.token.to_uint()              },

		{ "pc",                 entry.pc.to_uint()                 },
		{ "category",           to_string(entry.category)          },
		{ "func3",              entry.func3.to_uint()              },
		{ "is_func7_0b0000000", entry.is_func7_0b0000000.to_bool() },
		{ "is_func7_0b0000001", entry.is_func7_0b0000001.to_bool() },
		{ "is_func7_0b0100000", entry.is_func7_0b0100000.to_bool() },
		{ "packed_immediate",   entry.packed_immediate.to_uint()   },

		{ "use_dest",           entry.use_dest.to_bool()           },
		{ "use_src1",           entry.use_src1.to_bool()           },
		{ "use_src2",           entry.use_src2.to_bool()           },
		{ "dest",               entry.dest.to_uint()               },
		{ "src1",               entry.src1.to_uint()               },
		{ "src2",               entry.src2.to_uint()               },
		{ "src1_ready",         entry.src1_ready.to_bool()         },
		{ "src2_ready",         entry.src2_ready.to_bool()         }
	};
}

json to_json(const ReorderBufferEntry& entry) {
	return {
		{ "token",                entry.token.to_uint()                },
		{ "ready",                entry.ready.to_bool()                },
		{ "invalid",              entry.invalid.to_bool()              },

		{ "is_load",              entry.is_load.to_bool()              },
		{ "is_store",             entry.is_store.to_bool()             },
		{ "is_byte_operation",    entry.is_byte_operation.to_bool()    },
		{ "is_half_operation",    entry.is_half_operation.to_bool()    },
		{ "is_word_operation",    entry.is_word_operation.to_bool()    },
		{ "is_unsigned_extended", entry.is_unsigned_extended.to_bool() },
		{ "load_dest",            entry.load_dest.to_uint()            },
		{ "value",                entry.value.to_uint()                },
		{ "address",              entry.address.to_uint()              }
	};
}

/* ****************************************************************************
*    For inter-stage structures
**************************************************************************** */

json to_json(const FetchToDecode& fetch_to_decode) {
	return {
		{ "has_fetched",          fetch_to_decode.has_fetched.to_bool()             },
		{ "raw instruction",      string_hex(fetch_to_decode.instruction.to_uint()) },
		{ "pc",                   fetch_to_decode.pc.to_uint()                      }
	};
}

json to_json(const DecodeToFetch& decode_to_fetch) {
	return {
		{ "has_next_pc",          decode_to_fetch.has_next_pc.to_bool() },
		{ "next_pc",              decode_to_fetch.next_pc.to_uint()     }
	};
}

json to_json(const DecodeToIssue& decode_to_issue) {
	return {
		{ "has_decoded_instruction", decode_to_issue.has_decoded_instruction.to_bool() },
		{ "token",                   decode_to_issue.token.to_uint()                   },
		{ "pc",                      decode_to_issue.pc.to_uint()                      },
		{ "category",                to_string(decode_to_issue.category)               },
		{ "func3",                   decode_to_issue.func3.to_uint()                   },
		{ "is_func7_0b0000000",      decode_to_issue.is_func7_0b0000000.to_bool()      },
		{ "is_func7_0b0000001",      decode_to_issue.is_func7_0b0000001.to_bool()      },
		{ "is_func7_0b0100000",      decode_to_issue.is_func7_0b0100000.to_bool()      },
		{ "packed_immediate",        decode_to_issue.packed_immediate.to_uint()        },

		{ "use_dest",                decode_to_issue.use_dest.to_bool()                },
		{ "use_src1",                decode_to_issue.use_src1.to_bool()                },
		{ "use_src2",                decode_to_issue.use_src2.to_bool()                },
		{ "dest",                    decode_to_issue.dest.to_uint()                    },
		{ "src1",                    decode_to_issue.src1.to_uint()                    },
		{ "src2",                    decode_to_issue.src2.to_uint()                    },
		{ "src1_ready",              decode_to_issue.src1_ready.to_bool()              },
		{ "src2_ready",              decode_to_issue.src2_ready.to_bool()              }
	};
}

json to_json(const DecodeToCommit& decode_to_commit) {
	return {
		{ "add_to_rob", decode_to_commit.add_to_rob.to_bool() },
		{ "token",      decode_to_commit.token.to_uint()      },
		{ "invalid",    decode_to_commit.invalid.to_bool()    }
	};
}

json to_json(const IssueToWriteBack& issue_to_write_back) {
	return {
		{ "executed_instruction", issue_to_write_back.executed_instruction.to_bool() },
		{ "token",                issue_to_write_back.token.to_uint()                },

		{ "has_result",           issue_to_write_back.has_result.to_bool()           },
		{ "result",               issue_to_write_back.result.to_uint()               },
		{ "dest",                 issue_to_write_back.dest.to_uint()                 },
		{ "has_next_pc",          issue_to_write_back.has_next_pc.to_bool()          },
		{ "next_pc",              issue_to_write_back.next_pc.to_uint()              },
		{ "invalid",              issue_to_write_back.invalid.to_bool()              },

		{ "is_load",              issue_to_write_back.is_load.to_bool()              },
		{ "is_store",             issue_to_write_back.is_store.to_bool()             },
		{ "is_byte_operation",    issue_to_write_back.is_byte_operation.to_bool()    },
		{ "is_half_operation",    issue_to_write_back.is_half_operation.to_bool()    },
		{ "is_word_operation",    issue_to_write_back.is_word_operation.to_bool()    },
		{ "is_unsigned_extended", issue_to_write_back.is_unsigned_extended.to_bool() },
		{ "address",              issue_to_write_back.address.to_uint()              }
	};
}

json to_json(const WriteBackToFetch& write_back_to_fetch) {
	return {
		{ "has_next_pc", write_back_to_fetch.has_next_pc.to_bool() },
		{ "next_pc",     write_back_to_fetch.next_pc.to_uint()     }
	};
}

json to_json(const WriteBackToIssue& write_back_to_issue) {
	return {
		{ "has_a_src_ready", write_back_to_issue.has_a_src_ready.to_bool() },
		{ "src_ready",       write_back_to_issue.src_ready.to_uint()       },
		{ "result",          write_back_to_issue.result.to_uint()          }
	};
}

json to_json(const WriteBackToCommit& write_back_to_commit) {
	return {
		{ "executed_instruction", write_back_to_commit.executed_instruction.to_bool() },
		{ "token",                write_back_to_commit.token.to_uint()                },
		{ "invalid",              write_back_to_commit.invalid.to_bool()              },

		{ "is_load",              write_back_to_commit.is_load.to_bool()              },
		{ "is_store",             write_back_to_commit.is_store.to_bool()             },
		{ "is_byte_operation",    write_back_to_commit.is_byte_operation.to_bool()    },
		{ "is_half_operation",    write_back_to_commit.is_half_operation.to_bool()    },
		{ "is_word_operation",    write_back_to_commit.is_word_operation.to_bool()    },
		{ "is_unsigned_extended", write_back_to_commit.is_unsigned_extended.to_bool() },
		{ "load_dest",            write_back_to_commit.load_dest.to_uint()            },
		{ "value",                write_back_to_commit.value.to_uint()                },
		{ "address",              write_back_to_commit.address.to_uint()              }
	};
}

json to_json(const CommitToIssue& commit_to_issue) {
	return {
		{ "has_loaded_value", commit_to_issue.has_loaded_value.to_bool() },
		{ "dest",             commit_to_issue.dest.to_uint()             },
		{ "value",            commit_to_issue.value.to_uint()            }
	};
}

json to_json(const CommitToCommit& commit_to_commit) {
	return {
		{ "rob_was_empty",        commit_to_commit.rob_was_empty.to_bool()       },
		{ "previous_first_entry", to_json(commit_to_commit.previous_first_entry) }
	};
}

#endif // __SYNTHESIS__