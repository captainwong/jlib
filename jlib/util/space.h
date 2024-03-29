#pragma once

#include <string>
#include <cmath>

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
 */
static inline std::string human_readable_byte_count(uintmax_t bytes, PositionalNotation po = PositionalNotation::Binary)
{
    auto unit = po == PositionalNotation::Binary ? 1024 : 1000;
    if (bytes < unit) { return std::to_string(bytes) + "B"; }
    auto exp = static_cast<size_t>(std::log(bytes) / std::log(unit));
    auto pre = std::string("KMGTPEZYBND").at(exp - 1) + std::string(po == PositionalNotation::Binary ? "i" : "");
    auto var = bytes / std::pow(unit, exp);
	char buf[64] = { 0 };
	snprintf(buf, sizeof(buf), "%.2f%sB", var, pre.data());
    return buf;
}

}
