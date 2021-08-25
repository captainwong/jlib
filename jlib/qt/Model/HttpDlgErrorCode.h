#pragma once

#include <system_error>
#include <QString>
#include "../ErrorCode.h"

JLIBQT_NAMESPACE_BEGIN

enum class HttpDlgErrorCode {
	Timeout = 1,
	ParseJsonError,
	HttpStatusNeq200,
	Unknown,
};

JLIBQT_NAMESPACE_END

ENABLE_ENUM_AS_ERROR_CODE(JLIBQT_NAMESPACE HttpDlgErrorCode);
