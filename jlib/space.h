#pragma once

#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace jlib {

enum PositionalNotation {
    //! Used for memory size or Microsoft disk space
    Binary,
    //! Usually used by Disk Space
    Decimal,
};

/**
 * @brief Format byte count to human readable string
 * @note bytes must less than 1DB(DoggaByte)
 */
static std::string human_readable_byte_count(uintmax_t bytes, size_t precision = 1, PositionalNotation po = PositionalNotation::Binary)
{
    // http://programming.guide/java/formatting-byte-size-to-human-readable-format.html

    auto unit = po == PositionalNotation::Binary ? 1024 : 1000;
    if (bytes < unit) { return std::to_string(bytes) + "B"; }
    auto exp = static_cast<int>(std::log(bytes) / std::log(unit));
    auto pre = std::string("KMGTPEZYBND").at(exp - 1) + std::string(po == PositionalNotation::Binary ? "i" : "");
    auto var = bytes / std::pow(unit, exp);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << var << pre << "B";
    return ss.str();
}

}
