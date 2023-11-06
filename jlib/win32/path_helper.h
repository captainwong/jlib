#pragma once

#include "../base/config.h"
#include <string>
#include "path_op.h"
#include "../util/chrono_wrapper.h"


namespace jlib {
namespace win32 {


struct PathHelper
{
	static inline bool mkpath(const std::string& path) {
		return folderExists(path) ? true : createDirectory(path);
	}

	static inline bool mkpath(const std::wstring& path) {
		return folderExists(path) ? true : createDirectory(path);
	}

	static inline std::wstring currentTimeString() {
		return integrateFileName(nowToWString());
	}

	static inline std::string currentTimeStringA() {
		return integrateFileName(nowToString());
	}

	virtual ~PathHelper() {}

	virtual std::string program()	const { return programPath_; }
	virtual std::string exe()		const { return exePath_; }
	virtual std::string bin()		const { return program() + "\\bin"; }
	virtual std::string data()		const { return program() + "\\data"; }
	virtual std::string log()		const { return program() + "\\log"; }
	virtual std::string dumps()		const { return program() + "\\dumps"; }
	virtual std::string config()	const { return data() + "\\config"; }
	virtual std::string db()		const { return data() + "\\db"; }

protected:
	// disable constructor
	explicit PathHelper() {}
	//! 应用程序安装目录
	std::string programPath_ = {};
	//! 应用程序路径
	std::string exePath_ = {};
};

struct AutoSwithToBin {
	AutoSwithToBin(PathHelper* helper) : helper(helper) {}
	AutoSwithToBin(PathHelper* helper, const std::string& path) : helper(helper) {
		::SetCurrentDirectoryA(path.c_str());
	}
	~AutoSwithToBin() {
		if (helper) { ::SetCurrentDirectoryA(helper->bin().c_str()); }
	}

	PathHelper* helper = nullptr;
};

/*
* @brief 路径辅助类，数据保存在安装目录，程序和dll在bin文件夹
* @note 大致结构树为：
* @note |-- program-install-dir
* @note |   |-- bin
* @note |   |    |-- program.exe
* @note |   |    |-- *.dlls
* @note |   |-- dumps
* @note |   |-- log
* @note |   |-- data
* @note |       |-- config
* @note |       |-- db
*/
struct PathHelperLocal : PathHelper
{
	PathHelperLocal() : PathHelper() {
		exePath_ = getExePathA();
		programPath_ = getFolder(getFolder(exePath_));
	}
};


/*
* @brief 路径辅助类，数据保存在安装目录，程序和dll也在安装目录
* @note 大致结构树为：
* @note |-- program-install-dir
* @note |   |-- dumps
* @note |   |-- log
* @note |   |-- data
* @note |   |   |-- config
* @note |   |   |-- db
* @note |   |-- program.exe
* @note |   |-- *.dlls
*/
struct PathHelperLocalWithoutBin : PathHelper
{
	PathHelperLocalWithoutBin() : PathHelper() {
		exePath_ = getExePathA();
		programPath_ = getFolder(exePath_);
	}

	virtual std::string bin() const override { return program(); }
};

/*
* @brief 路径辅助类，数据保存在其他可写目录，如 C:/Users/[USER]/AppData/Roaming
* @note 大致结构树为：
* @note |-- program-install-dir
* @note |   |-- bin
* @note |   |    |-- program.exe
* @note |   |    |-- *.dlls
* @note
* @note |-- C:/Users/[USER]/AppData/Roaming
* @note |   |-- program-name
* @note |        |-- log
* @note |        |-- dumps
* @note |        |-- config
* @note |        |-- db
*/
struct PathHelperDataSeperated : PathHelperLocal
{
	PathHelperDataSeperated(const std::string& appName, bool useTmpAsLogFolder = false)
		: PathHelperLocal(), appName(appName), useTmpAsLogFolder(useTmpAsLogFolder)
	{}

	virtual std::string data()		const override {
		auto path = getAppDataPathA() + "\\" + appName; 
		mkpath(path); return path;		
	}
	virtual std::string log()		const override { return useTmpAsLogFolder ? getTempPathA() : (data() + "\\log"); }
	virtual std::string dumps()		const override { return data() + "\\dumps"; }
	virtual std::string config()	const override { return data() + "\\config"; }
	virtual std::string db()		const override { return data() + "\\db"; }

protected:
	std::string appName = {};
	bool useTmpAsLogFolder = false;
};

/*
* @brief 路径辅助类，数据保存在其他可写目录，如 C:/Users/[USER]/AppData/Roaming
* @note 大致结构树为：
* @note |-- program-install-dir
* @note |   |-- program.exe
* @note |   |-- *.dlls
* @note
* @note |-- C:/Users/[USER]/AppData/Roaming
* @note |   |-- program-name
* @note |        |-- log
* @note |        |-- dumps
* @note |        |-- config
* @note |        |-- db
*/
struct PathHelperDataSeperatedWithoutBin : PathHelperLocalWithoutBin
{
	PathHelperDataSeperatedWithoutBin(const std::string& appName, bool useTmpAsLogFolder = false)
		: PathHelperLocalWithoutBin(), appName(appName), useTmpAsLogFolder(useTmpAsLogFolder)
	{}

	virtual std::string data()		const override {
		auto path = getAppDataPathA() + "\\" + appName;
		mkpath(path); return path;
	}
	virtual std::string log()		const override { return useTmpAsLogFolder ? getTempPathA() : (data() + "\\log"); }
	virtual std::string dumps()		const override { return data() + "\\dumps"; }
	virtual std::string config()	const override { return data() + "\\config"; }
	virtual std::string db()		const override { return data() + "\\db"; }

protected:
	std::string appName = {};
	bool useTmpAsLogFolder = false;
};

}
}
