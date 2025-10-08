#ifndef __HEX_H__
#define __HEX_H__

#include <assert.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "str_util.h"

namespace jlib {

// xdigit to decimal
inline uint8_t x2d(uint8_t ascii) {
    if ('0' <= ascii && ascii <= '9') {
        return ascii - '0';
    }
    if ('A' <= ascii && ascii <= 'F') {
        return ascii - 'A' + 10;
    }
    if ('a' <= ascii && ascii <= 'f') {
        return ascii - 'a' + 10;
    }
    assert(false);
    return 0;
}

template <typename value_type = uint8_t>
inline bool read_hex(const char* str, size_t len, value_type& hex) {
    if (len < sizeof(value_type) * 2) {
        return false;
    }
    hex = 0;
    for (size_t i = 0; i < sizeof(value_type) * 2; i++) {
        if (!isxdigit(str[i])) {
            return false;
        } else {
            hex = (hex << 4) | x2d(str[i]);
        }
    }
    return true;
}

// convert ascii-hex format to binary
// the ascii-hex can contains spaces as separators
// return 0 for success
inline int hex_to_bin(const std::string& hex_content, std::vector<uint8_t>& bin_data) {
    auto hex = jlib::erase_all_copy(hex_content, ' ');
    jlib::erase_all(hex, '\t');
    jlib::erase_all(hex, '\r');
    jlib::erase_all(hex, '\n');

    bin_data.clear();
    if (hex.empty() || (hex.length() % 2 != 0)) {
        return 0;
    }

    const char* p = hex.c_str();
    size_t len = hex.size();
    while (len > 0) {
        uint8_t byte = 0;
        if (!read_hex(p, len, byte)) {
            return -1;
        }
        bin_data.push_back(byte);
        p += 2;
        len -= 2;
    }
    return 0;
}

}  // namespace jlib

#endif /* __HEX_H__ */
