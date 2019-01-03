#pragma once

#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>

namespace jlib {

struct version_no {
	int major = 0;
	int minor = 0;
	int revision = 0;
	int build = 0;

	version_no& from_string(const std::string& s) {
		std::sscanf(s.c_str(), "%d.%d.%d.%d", &major, &minor, &revision, &build);
		if (major < 0) major = 0;
		if (minor < 0) minor = 0;
		if (revision < 0) revision = 0;
		if (build < 0) build = 0;
		return *this;
	}

	std::string to_string() const {
		std::stringstream ss;
		ss << major << "." << minor << "." << revision << "." << build;
		return ss.str();
	}

	bool valid() {
		return !(major == 0 && minor == 0 && revision == 0 && build == 0);
	}

	void reset() {
		major = minor = revision = build = 0;
	}

	bool operator == (const version_no& ver) {
		return major == ver.major
			&& minor == ver.minor
			&& revision == ver.revision
			&& build == ver.build;
	}

	bool operator < (const version_no& ver) {
		if (major > ver.major) return false;
		if (minor > ver.minor) return false;
		if (revision > ver.revision) return false;
		if (build > ver.build) return false;
		if (this->operator==(ver)) return false;
		return true;
	}


};

struct update_info_json {
	version_no ver;
	std::string change;
	std::vector<std::string> dllinks;
};

struct update_info_text {
	version_no ver;
	std::string dllink;
};

inline bool get_version_no_from_file(version_no& ver, const std::string& file_path) {
	std::ifstream in(file_path);
	if (!in)return false;
	std::stringstream is;
	is << in.rdbuf();
	version_no file_ver;
	file_ver.from_string(is.str());
	if (file_ver.valid()) {
		ver = file_ver;
		return true;
	}
	return false;
}


}
