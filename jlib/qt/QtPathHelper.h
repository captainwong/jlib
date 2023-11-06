#pragma once

#include "qt_global.h"
#include <QDir>
#include <QCoreApplication>
#include <QString>
#include <QDateTime>
#include <QStandardPaths>

JLIBQT_NAMESPACE_BEGIN

static const auto JLIBQT_FILE_TIME_FORMAT = "yyyy-MM-dd_hh-mm-ss";

struct PathHelper
{
	static inline bool mkpath(const QString& path) {
		QDir dir; return dir.exists(path) ? true : dir.mkpath(path);
	}

	static inline QString currentTimeString() {
		return QDateTime::currentDateTime().toString(JLIBQT_FILE_TIME_FORMAT);
	}

	static inline QString getFolder(const QString& path) {
		QDir dir(path); dir.cdUp();
		return dir.absolutePath();
	}

	virtual ~PathHelper() {}

	virtual QString program()	const { return programPath_; }
	virtual QString exe()		const { return exePath_; }
	virtual QString bin()		const { return program() + "/bin"; }
	virtual QString data()		const { return program() + "/data"; }
	virtual QString log()		const { return program() + "/log"; }
	virtual QString dumps()		const { return program() + "/dumps"; }
	virtual QString config()	const { return data() + "/config"; }
	virtual QString db()		const { return data() + "/db"; }
	
protected:
	// disable constructor
	explicit PathHelper() {}
	//! 应用程序安装目录
	QString programPath_ = {};
	//! 应用程序路径
	QString exePath_ = {};
};

struct AutoSwithToBin {
	AutoSwithToBin(PathHelper* helper) : helper(helper) {}
	AutoSwithToBin(PathHelper* helper, const QString& path) : helper(helper) {
		QDir().setCurrent(path);
	}
	~AutoSwithToBin() {
		if (helper) { QDir().setCurrent(helper->bin()); }
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
		exePath_ = QCoreApplication::applicationFilePath();
		QDir dir(QCoreApplication::applicationDirPath());
		dir.cdUp(); programPath_ = dir.absolutePath();
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
		exePath_ = QCoreApplication::applicationFilePath();
		QDir dir(QCoreApplication::applicationDirPath());
		programPath_ = dir.absolutePath();
	}

	virtual QString bin() const override { return program(); }
};

/*
* @brief 路径辅助类，数据保存在其他可写目录，如 C:/Users/[USER]/AppData/Roaming
* @note program-name 无需设置，Qt会自动设置
* @note 如果在调用此类之前先调用了 QCoreApplication::setOrganizationName("your-organization-name");
* @note 那么program-name在your-organization-name下
* @note 大致结构树为：
* @note |-- program-install-dir
* @note |   |-- bin
* @note |   |    |-- program.exe
* @note |   |    |-- *.dlls
* @note
* @note |-- writable-dir
* @note |   |-- your-organization-name
* @note |   |    |-- program-name
* @note |   |        |-- log
* @note |   |        |-- dumps
* @note |   |        |-- config
* @note |   |        |-- db
*/
struct PathHelperDataSeperated : PathHelperLocal
{
	PathHelperDataSeperated(bool useTmpAsLogFolder = false)
		: PathHelperLocal(), useTmpAsLogFolder(useTmpAsLogFolder)
	{}

	virtual QString data()		const override {
		return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	}
	virtual QString log()		const override { return useTmpAsLogFolder ? QStandardPaths::writableLocation(QStandardPaths::TempLocation) : (data() + "/log"); }
	virtual QString dumps()		const override { return data() + "/dumps"; }
	virtual QString config()	const override { return data() + "/config"; }
	virtual QString db()		const override { return data() + "/db"; }

protected:
	bool useTmpAsLogFolder = false;
};

/*
* @brief 路径辅助类，数据保存在其他可写目录，如 C:/Users/[USER]/AppData/Roaming
* @note program-name 无需设置，Qt会自动设置
* @note 如果在调用此类之前先调用了 QCoreApplication::setOrganizationName("your-organization-name");
* @note 那么program-name在your-organization-name下
* @note 大致结构树为：
* @note |-- program-install-dir
* @note |   |-- program.exe
* @note |   |-- *.dlls
* @note
* @note |-- writable-dir
* @note |   |-- your-organization-name
* @note |   |    |-- program-name
* @note |   |        |-- log
* @note |   |        |-- dumps
* @note |   |        |-- config
* @note |   |        |-- db
*/
struct PathHelperDataSeperatedWithoutBin : PathHelperLocalWithoutBin
{
	PathHelperDataSeperatedWithoutBin(bool useTmpAsLogFolder = false)
		: PathHelperLocalWithoutBin(), useTmpAsLogFolder(useTmpAsLogFolder)
	{}

	virtual QString data()		const override {
		return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	}
	virtual QString log()		const override { return useTmpAsLogFolder ? QStandardPaths::writableLocation(QStandardPaths::TempLocation) : (data() + "/log"); }
	virtual QString dumps()		const override { return data() + "/dumps"; }
	virtual QString config()	const override { return data() + "/config"; }
	virtual QString db()		const override { return data() + "/db"; }

protected:
	bool useTmpAsLogFolder = false;
};


JLIBQT_NAMESPACE_END
