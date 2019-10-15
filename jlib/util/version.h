#pragma once

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

	Version& fromString(const std::string& s) {
		if (std::sscanf(s.c_str(), "%d.%d.%d.%d", &major, &minor, &revision, &build) != 4) {
			reset(); return *this;
		}
		if (major < 0) major = 0;
		if (minor < 0) minor = 0;
		if (revision < 0) revision = 0;
		if (build < 0) build = 0;
		return *this;
	}

	std::string toString() const {
		return std::to_string(major) + "."
			+ std::to_string(minor) + "."
			+ std::to_string(revision) + "."
			+ std::to_string(build);
	}

	bool valid() {
		return !(major == 0 && minor == 0 && revision == 0 && build == 0);
	}

	void reset() {
		major = minor = revision = build = 0;
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

inline bool getVersionFromFile(const std::string& file_path, Version& version) {
	std::ifstream in(file_path);
	if (!in)return false;
	std::stringstream is;
	is << in.rdbuf();
	Version file_ver;
	file_ver.fromString(is.str());
	if (file_ver.valid()) {
		version = file_ver;
		return true;
	}
	return false;
}

}
