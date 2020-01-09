#pragma once

#include "qt_global.h"
#include <system_error>
#include <QString>


JLIBQT_NAMESPACE_BEGIN

//! ��ȡ��������������
inline QString ecString(const std::error_code& ec) {
	return QString::fromLocal8Bit(ec.message().data());
}

//! ��ȡ�������������������д���Ŀ¼��
inline QString ecStringWithCategory(const std::error_code& ec) {
	return QString::fromLocal8Bit(ec.category().name()) + ": " + QString::fromLocal8Bit(ec.message().data());
}

#define BREAK_IF_QUERY_FAILED(ec_type) if (!ok) { \
	ec = ec_type; \
	MYQCRITICAL << ecStringWithCategory(ec) << "\n" << query.lastError(); \
	break; \
}

enum class CommonErrorCode {
	OpenDbFailed = 1,
	CreateDbTableFailed,
	QueryDbFailed,

};

JLIBQT_NAMESPACE_END

namespace std
{

#define ENABLE_ENUM_AS_ERROR_CODE(type) \
template <> struct is_error_code_enum<type> : true_type {}; \
std::error_code make_error_code(type);

ENABLE_ENUM_AS_ERROR_CODE(JLIBQT_NAMESPACE CommonErrorCode)

}

namespace
{

struct CommonErrorCatagory : public std::error_category
{
	const char* name() const noexcept override {
		return "Common Error";
	}

	std::string message(int ev) const override {
		switch (static_cast<JLIBQT_NAMESPACE CommonErrorCode>(ev)) {
			case JLIBQT_NAMESPACE CommonErrorCode::OpenDbFailed:
				return QObject::tr("Failed to open database").toLocal8Bit().toStdString();
			case JLIBQT_NAMESPACE CommonErrorCode::CreateDbTableFailed:
				return QObject::tr("Failed to create table").toLocal8Bit().toStdString();
			case JLIBQT_NAMESPACE CommonErrorCode::QueryDbFailed:
				return QObject::tr("Failed to query database").toLocal8Bit().toStdString();
			default:
				return QObject::tr("Unkown Error").toLocal8Bit().toStdString();
		}
	}
};

static const CommonErrorCatagory theCommonErrorCatagory = {};

}

inline std::error_code std::make_error_code(JLIBQT_NAMESPACE CommonErrorCode ec)
{
	return { static_cast<int>(ec), theCommonErrorCatagory };
}



