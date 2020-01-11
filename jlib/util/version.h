#pragma once

#include "../base/config.h"
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>

namespace jlib
{

//! 应用程序版本分支
enum class Branch : int {
	//! 测试版，一般仅用于测试人员测试
	Test,
	//! 体验版，可以投放给少量用户进行灰度测试
	Experimental,
	//! 正式版
	Stable,

	BranchCount,

	InvalidBranch = 0x0FFFFFFF,
};

static constexpr const char* BranchNameTest			= "test";
static constexpr const char* BranchNameExperimental = "experimental";
static constexpr const char* BranchNameStable		= "stable";

inline const char* branchName(Branch branch) {
	switch (branch) {
	case jlib::Branch::Test:			return BranchNameTest;
	case jlib::Branch::Experimental:	return BranchNameExperimental;
	case jlib::Branch::Stable:			return BranchNameStable;
	default:							return "InvalidBranch";
	}
}

inline Branch branchFromString(const std::string& name) {
	if (name == BranchNameTest) { return Branch::Test; }
	else if (name == BranchNameExperimental) { return Branch::Experimental; }
	else if (name == BranchNameStable) { return Branch::Stable; }
	else { return Branch::InvalidBranch; }
}

//! 应用程序版本号
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

	bool valid() const { return !(major == 0 && minor == 0 && revision == 0 && build == 0); }
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

	bool operator == (const Version& ver) const {
		return major == ver.major
			&& minor == ver.minor
			&& revision == ver.revision
			&& build == ver.build;
	}

	bool operator < (const Version& ver) const {
		if (major > ver.major) return false;
		if (minor > ver.minor) return false;
		if (revision > ver.revision) return false;
		if (build > ver.build) return false;
		if (this->operator==(ver)) return false;
		return true;
	}

	bool operator > (const Version& ver) const {
		if (major < ver.major) return false;
		if (minor < ver.minor) return false;
		if (revision < ver.revision) return false;
		if (build < ver.build) return false;
		if (this->operator==(ver)) return false;
		return true;
	}

	bool operator <= (const Version& ver) const {
		return (major <= ver.major) 
			&& (minor <= ver.minor) 
			&& (revision <= ver.revision) 
			&& (build <= ver.build);
	}

	bool operator >= (const Version& ver) const {
		return (major >= ver.major)
			&& (minor >= ver.minor)
			&& (revision >= ver.revision)
			&& (build >= ver.build);
	}
};

struct DownLoadLink {
	std::string host = {}; // local, qcloud, ...
	std::string link = {};
	std::string md5 = {};
};

//! 发布
struct Release {
	Branch branch = Branch::InvalidBranch;
	Version version = {}; 
	std::string change = {};
	std::vector<DownLoadLink> dllinks = {};

	std::string toString() const {
		auto s = std::string(branchName(branch)) + " " + version.toString() + "\nchange=" + change + "\ndownload_links=\n";
		if (!dllinks.empty()) {
			for (const auto& link : dllinks) { s += link.host + " : link=" + link.link + " md5=" + link.md5 + "\n"; }
		} else { s += "empty"; }
		return s;
	}

	bool valid() const {
		return (branch != Branch::InvalidBranch) && version.valid() && !dllinks.empty();
	}
};

//! 最新的发布
struct LatestRelease {
	std::vector<Release> releases = {};

	std::string toString() const {
		std::string str;
		for (const auto& r : releases) {
			if (r.valid()) { str += r.toString() + "\n"; }
		}
		return str;
	}
};

//! value 为从服务器获取的最新发布信息，以 JSON 表示
template <typename JsonValue>
int resolveLatestRelease(const JsonValue& value, LatestRelease& latestRelease) {
	int count = 0;
	for (int i = 0; i < (int)Branch::BranchCount; i++) {
		Branch branch = Branch(i);
		if (value.isMember(branchName(branch))) {
			const auto& detail = value[BranchNameTest];
			Release release;
			release.branch = branch;
			release.version = detail["version"].asString();
			release.change = detail["release_note"].asString();
			if (detail["download_links"].isMember("local")) {
				DownLoadLink dl;
				dl.host = "local";
				dl.link = detail["download_links"]["local"]["link"].asString();
				dl.md5 = detail["download_links"]["local"]["md5"].asString();
				release.dllinks.push_back(dl);
			}
			if (detail["download_links"].isMember("qcloud")) {
				DownLoadLink dl;
				dl.host = "qcloud";
				dl.link = detail["download_links"]["qcloud"]["link"].asString();
				dl.md5 = detail["download_links"]["qcloud"]["md5"].asString();
				release.dllinks.push_back(dl);
			}
			latestRelease.releases.push_back(release);
			count++;
		}
	}
	
	return count;
}

}
