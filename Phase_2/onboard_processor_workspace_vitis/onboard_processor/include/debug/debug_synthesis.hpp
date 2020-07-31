#pragma once

/* ****************************************************************************
*    Header file containing the definition of the debug structure for
*    synthesis
**************************************************************************** */

#include "sleep.h"
#include "xprocessor.h"

#include <array>
#include <iostream>
#include <streambuf>

struct custom_streambuf : std::streambuf {
    static const uint32_t buf_size = 4096;
    static const uint32_t chunk_size = 2048;
    using buffer_t = std::array<char_type, buf_size>;
	buffer_t* buf;
    uint32_t ptr = 0;
public:
    custom_streambuf(buffer_t* buf_) {
		buf = buf_;
        buf->fill(0);
    }

    ~custom_streambuf() {
        flush();
    }
protected:
    void flush() {
        uint32_t todo = ptr;
        uint32_t todo_chunk = todo == 0 ? 0 : (todo % chunk_size == 0 ? (todo / chunk_size) : (todo / chunk_size + 1));
        // std::cout << "flushing... \n";
        for (uint16_t i = 0; i < todo_chunk; i++) {
            for (uint16_t j = 0; j < chunk_size; j++) {
                std::cout << buf->at(i * chunk_size + j);
            }
            // std::cout << "chunck ends...\n";
            sleep(1); // sleep 1 second
        }
        ptr = 0;
    }

    int sync() override {
        // std::cout << "sync... \n";
        flush();
        return 0;
    }

    std::streamsize xsputn(const char_type* s, std::streamsize n) override {
        // std::cout << "xsputn..." << n << "\n";
        std::streamsize m = n;
        uint32_t s_index = 0;
        while (m > 0) {
            std::streamsize diff = buf_size - ptr;
            uint32_t todo = std::min(m, diff);
            for (uint32_t i = 0; i < todo; i++) {
                buf->at(ptr + i) = s[s_index + i];
            }
            ptr += todo;
            s_index += todo;
            m -= todo;
            if (ptr == buf_size)
                flush();
        }
        return n;
    }

    int_type overflow(int_type ch) override {
        buf->at(ptr) = ch;
        ptr++;
        if (ptr == buf_size)
            flush();
        return 1;
    }
};

void dumpDebugInfo(XProcessor* instance);