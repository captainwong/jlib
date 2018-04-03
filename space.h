#pragma once

#include <string>

namespace jlib {

static std::string format_space(uintmax_t bytes)
{
    static constexpr uintmax_t FACTOR = 1024;
	static constexpr uintmax_t KB = FACTOR;
	static constexpr uintmax_t MB = KB * FACTOR;
	static constexpr uintmax_t GB = MB * FACTOR;
	static constexpr uintmax_t TB = GB * FACTOR;

	uintmax_t kb = bytes / FACTOR;
	uintmax_t mb = kb / FACTOR;
	uintmax_t gb = mb / FACTOR;
	uintmax_t tb = gb / FACTOR;

	std::string s;
	if (tb > 0) {
		gb -= tb * FACTOR;
		gb = gb * 1000 / FACTOR;
		gb /= 10;
		return std::to_string(tb) + "." + std::to_string(gb) + "T";
	} else if (gb > 0) {
		mb -= gb * FACTOR;
		mb = mb * 1000 / FACTOR;
		mb /= 10;
		return std::to_string(gb) + "." + std::to_string(mb) + "G";
	} else if (mb > 0) {
		kb -= mb * FACTOR;
		kb = kb * 1000 / FACTOR;
		kb /= 10;
		return std::to_string(mb) + "." + std::to_string(kb) + "M";
	} else if (kb > 0) {
		bytes -= kb * FACTOR;
		bytes = bytes * 1000 / FACTOR;
		bytes /= 10;
		return std::to_string(kb) + "." + std::to_string(bytes) + "K";
	} else {
		return std::to_string(bytes) + "B";
	}
}

}
