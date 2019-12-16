#pragma once

#include "../base/config.h"
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>

namespace jlib
{

struct Version {
	int major = 0;
	int minor = 0;
	int revision = 0;
	int build = 0;

	Version() = default;
	Version(int major, int minor, int revision, int build)
		: major(major), minor(minor), revision(revision), build(build) {}
	Version(const std::string& s) { _fromString(s); }
	Version& fromString(const std::string& s) { _fromString(s); return *this; }

	bool valid() { return !(major == 0 && minor == 0 && revision == 0 && build == 0); }
	void reset() { major = minor = revision = build = 0; }

	bool _fromString(const std::string& s) {
		if (std::sscanf(s.c_str(), "%d.%d.%d.%d", &major, &minor, &revision, &build) != 4) {
			reset(); return false;
		}
		if (major < 0) major = 0; if (minor < 0) minor = 0;
		if (revision < 0) revision = 0; if (build < 0) build = 0;
		return true;
	}

	std::string toString() const {
		return std::to_string(major) + "." +
			std::to_string(minor) + "." +
			std::to_string(revision) + "." +
			std::to_string(build);
	}

	template <typename T>
	bool fromFile(T const & path) {
		if constexpr (std::is_convertible_v<T, std::string>);
		else if constexpr (std::is_convertible_v<T, std::wstring>);
		else { return false; }
		std::ifstream in(path); if (!in) return false;
		std::stringstream is; is << in.rdbuf();
		Version file_ver(is.str());
		if (file_ver.valid()) { *this = file_ver; return true; }
		return false;
	}

	template <typename T>
	bool toFile(T const& path) {
		if constexpr (std::is_convertible_v<T, std::string>);
		else if constexpr (std::is_convertible_v<T, std::wstring>);
		else { return false; }
		std::ofstream out(path); if (!out) { return false; }
		auto str = toString(); out.write(str.data(), str.size());
		return true;
	}

	bool operator == (const Version& ver) {
		return major == ver.major
			&& minor == ver.minor
			&& revision == ver.revision
			&& build == ver.build;
	}

	bool operator < (const Version& ver) {
		if (major > ver.major) return false;
		if (minor > ver.minor) return false;
		if (revision > ver.revision) return false;
		if (build > ver.build) return false;
		if (this->operator==(ver)) return false;
		return true;
	}

	bool operator > (const Version& ver) {
		if (major < ver.major) return false;
		if (minor < ver.minor) return false;
		if (revision < ver.revision) return false;
		if (build < ver.build) return false;
		if (this->operator==(ver)) return false;
		return true;
	}

	bool operator <= (const Version& ver) {
		return (major <= ver.major) 
			|| (minor <= ver.minor) 
			|| (revision <= ver.revision) 
			|| (build <= ver.build);
	}

	bool operator >= (const Version& ver) {
		return (major >= ver.major)
			|| (minor >= ver.minor)
			|| (revision >= ver.revision)
			|| (build >= ver.build);
	}
};

struct UpdateInfo {
	Version version = {};
	std::string change = {};
	std::vector<std::string> dllinks = {};
};

struct UpdateInfoText {
	Version version = {};
	std::string dllink = {};
};

}
