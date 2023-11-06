#pragma once

#include "../base/config.h"
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdint.h>

namespace jlib
{

//! 应用程序版本分支
enum class Branch : int {
	// 开发版，开发人员自测
	Dev,
	//! 测试版，一般仅用于测试人员测试
	Test,
	//! 体验版，可以投放给少量用户进行灰度测试
	Experimental,
	//! 正式版
	Stable,

	BranchCount,

	InvalidBranch = 0x0FFFFFFF,
};

static constexpr auto BranchNameDev          = "dev";
static constexpr auto BranchNameTest		 = "test";
static constexpr auto BranchNameExperimental = "experimental";
static constexpr auto BranchNameStable		 = "stable";

inline const char* branchName(Branch branch) {
	switch (branch) {
	case jlib::Branch::Dev:				return BranchNameDev;
	case jlib::Branch::Test:			return BranchNameTest;
	case jlib::Branch::Experimental:	return BranchNameExperimental;
	case jlib::Branch::Stable:			return BranchNameStable;
	default:							return "InvalidBranch";
	}
}

inline Branch branchFromString(const std::string& name) {
	if (name == BranchNameDev) { return Branch::Dev; }
	else if (name == BranchNameTest) { return Branch::Test; }
	else if (name == BranchNameExperimental) { return Branch::Experimental; }
	else if (name == BranchNameStable) { return Branch::Stable; }
	else { return Branch::InvalidBranch; }
}

//! 应用程序版本号
struct Version {
	//! 主版本
	uint16_t major = 0;
	//! 功能更新
	uint16_t minor = 0;
	//! bug 修复
	uint16_t revision = 0;
	//! 随意，我一般用来表示程序总迭代（编译成功）次数
	uint16_t build = 0;

	Version() = default;
	Version(uint16_t major, uint16_t minor, uint16_t revision, uint16_t build)
		: major(major), minor(minor), revision(revision), build(build) {}
	Version(const std::string& s) { _fromString(s); }
	Version(uint64_t v) { fromUInt64(v); }
	Version& fromString(const std::string& s) { _fromString(s); return *this; }
	Version& operator=(const std::string& s) { _fromString(s); return *this; }	
	Version& operator=(uint64_t v) { fromUInt64(v); return *this; }

	bool valid() const { return !(major == 0 && minor == 0 && revision == 0 && build == 0); }
	void reset() { major = minor = revision = build = 0; }

	bool _fromString(const std::string& s) {
		if (std::sscanf(s.c_str(), "%hu.%hu.%hu.%hu", &major, &minor, &revision, &build) != 4) {
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

	void fromUInt64(uint64_t v) {
		major = (v >> 48) & 0xFFFF;
		minor = (v >> 32) & 0xFFFF;
		revision = (v >> 16) & 0xFFFF;
		build = v & 0xFFFF;
	}

	uint64_t toUInt64() const {
		return ((uint64_t)major << 48) | ((uint64_t)minor << 32) | ((uint64_t)revision << 16) | (uint64_t)build;
	}	

	//! require cpp17
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

	//! require cpp17
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
		return toUInt64() == ver.toUInt64();
	}

	bool operator < (const Version& ver) const {
		return toUInt64() < ver.toUInt64();
	}

	bool operator > (const Version& ver) const {
		return toUInt64() > ver.toUInt64();
	}

	bool operator <= (const Version& ver) const {
		return toUInt64() <= ver.toUInt64();
	}

	bool operator >= (const Version& ver) const {
		return toUInt64() >= ver.toUInt64();
	}
};

//! 安装包下载链接
struct DownLoadLink {
	std::string host = {}; // local, qcloud, aliyun, ...
	std::string link = {};
	std::string md5 = {};
};

//! 发布
struct Release {
	Branch branch = Branch::InvalidBranch;
	Version version = {}; 
	bool force_update = false;
	//! 版本更新说明
	std::string change = {};
	std::vector<DownLoadLink> dllinks = {};

	std::string toString() const {
		auto s = std::string(branchName(branch)) + " " + version.toString() + 
			"\nforce_update=" + (force_update ? "true" : "false") + 
			"\nchange=" + change + 
			"\ndownload_links=\n";
		if (!dllinks.empty()) {
			for (const auto& link : dllinks) { s += link.host + " : link=" + link.link + " md5=" + link.md5 + "\n"; }
		} else { s += "empty"; }
		return s;
	}

	bool valid() const {
		return (branch != Branch::InvalidBranch) && version.valid() && !dllinks.empty();
	}
};

//! 最新的发布，包含各个分支的最新版本
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

/*
* @brief 从服务器获取最新版本信息，包含各个分支的最新版本
* @param value 为从服务器获取的最新发布信息，以 JSON 表示
* @param latestReleases 版本信息
* @return count 
* @note 示例数据
* @code{.json}
* {
* 	"test": {
* 		"version": "2.2.11.13262",
*		"force_update": true,
* 		"release_note": "测试;",
* 		"download_links": {
* 			"local": {
* 				"link": "https://your-host.com/download/installer.exe",
* 				"md5": "06907d59d7a656be62fc93bbf867604b"
* 			}
* 		}
* 	},
* 	"stable": {
* 		"version": "2.2.12.13282",
*		"force_update": false,
* 		"release_note": "测试稳定",
* 		"download_links": {
* 			"local": {
* 				"link": "https://your-host.com/download/installer.exe",
* 				"md5": "06907d59d7a656be62fc93bbf867604b"
* 			},
*	        "qcloud": {
* 				"link": "https://xxx.file.myqcloud.com/xxx/installer.exe",
* 				"md5": "06907d59d7a656be62fc93bbf867604b"
* 			}
* 		}
* 	}
* }
* @endcode
*/
template <typename JsonValue>
int resolveLatestRelease(const JsonValue& value, LatestRelease& latestRelease) {
	int count = 0;
	for (int i = 0; i < (int)Branch::BranchCount; i++) {
		Branch branch = Branch(i);
		if (value.isMember(branchName(branch))) {
			const auto& detail = value[branchName(branch)];
			Release release;
			release.branch = branch;
			release.force_update = detail.isMember("force_update") ? detail["force_update"].asBool() : false;
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

} // end namespace jlib
