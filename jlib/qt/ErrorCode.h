#pragma once

#include "qt_global.h"
#include <system_error>
#include <QString>


JLIBQT_NAMESPACE_BEGIN

//! 获取错误码文字描述
inline QString ecString(const std::error_code& ec) {
	return QString::fromLocal8Bit(ec.message().data());
}

//! 获取错误码文字描述（带有错误目录）
inline QString ecStringWithCategory(const std::error_code& ec) {
	return QString::fromLocal8Bit(ec.category().name()) + ": " + QString::fromLocal8Bit(ec.message().data());
}

#define BREAK_IF_QUERY_FAILED(ec_type) if (!ok) { \
	ec = ec_type; \
	MYQCRITICAL << ecStringWithCategory(ec) << "\n" << query.lastError(); \
	break; \
}

enum class DatabaseErrorCode {
	OpenDbFailed = 1,
	CreateDbTableFailed,
	QueryDbFailed,

};

JLIBQT_NAMESPACE_END


#define ENABLE_ENUM_AS_ERROR_CODE(type) \
namespace std { \
template <> struct is_error_code_enum<type> : true_type {}; \
std::error_code make_error_code(type); \
}

ENABLE_ENUM_AS_ERROR_CODE(JLIBQT_NAMESPACE DatabaseErrorCode)



std::error_code std::make_error_code(JLIBQT_NAMESPACE DatabaseErrorCode ec);

