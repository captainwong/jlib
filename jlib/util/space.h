#pragma once

#include <cmath>
#include <string>

namespace jlib {

enum class PositionalNotation {
    //! Used for memory size or Microsoft disk space
    Binary,
    //! Usually used by Disk Space
    Decimal,
};

/**
 * @brief Format byte count to human readable string
 * @note bytes must less than 1DB(DoggaByte)
 * @note http://programming.guide/java/formatting-byte-size-to-human-readable-format.html
 * @param bytes Byte count
 * @param po Positional notation, binary(1024) or decimal(1000)
 * @param show_i Whether to show 'i' in the unit when using binary notation
 * @return Formatted string
 */
inline std::string human_readable_byte_count(uintmax_t bytes, PositionalNotation po = PositionalNotation::Binary, bool show_i = false) {
    auto unit = po == PositionalNotation::Binary ? 1024 : 1000;
    if (bytes < unit) {
        return std::to_string(bytes) + "B";
    }
    auto exp = static_cast<size_t>(std::log(bytes) / std::log(unit));
    auto pre = std::string("KMGTPEZYBND").at(exp - 1) + std::string((po == PositionalNotation::Binary && show_i) ? "i" : "");
    auto var = bytes / std::pow(unit, exp);
    char buf[64] = {0};
    snprintf(buf, sizeof(buf), "%.2f", var);
    // trim trailing zeros and decimal point if needed
    for (int i = static_cast<int>(strlen(buf)) - 1; i > 0; --i) {
        if (buf[i] == '0') {
            buf[i] = '\0';
        } else if (buf[i] == '.') {
            buf[i] = '\0';
            break;
        } else {
            break;
        }
    }
    return std::string(buf) + pre + "B";
}

}  // namespace jlib
