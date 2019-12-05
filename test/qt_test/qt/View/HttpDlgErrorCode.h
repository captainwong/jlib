#pragma once
#include <QString>
#include <QObject>
#include "../ErrorCode.h"

namespace jlib
{
namespace qt
{

enum class HttpDlgErrorCode {
	NetworkError = 1,
	ParseJsonError,

};

}
}

namespace std {
ENABLE_ENUM_AS_ERROR_CODE(jlib::qt::HttpDlgErrorCode);
}


namespace
{

class HttpDlgErrorCategory : public std::error_category
{
public:
	const char* name() const noexcept override {
		return "HttpClient";
	}

	std::string message(int ev) const override {
		switch (static_cast<jlib::qt::HttpDlgErrorCode>(ev)) {
			case jlib::qt::HttpDlgErrorCode::NetworkError:
				return QObject::tr("Network Error").toLocal8Bit().toStdString();
				break;
			case jlib::qt::HttpDlgErrorCode::ParseJsonError:
				return QObject::tr("Cannot Parse Json").toLocal8Bit().toStdString();
				break;
			default:
				return QObject::tr("Unkown Error").toLocal8Bit().toStdString();
				break;
		}
	}
};

static const HttpDlgErrorCategory theHttpDlgErrorCategory = {};

}

inline std::error_code std::make_error_code(jlib::qt::HttpDlgErrorCode ec)
{
	return { static_cast<int>(ec), theHttpDlgErrorCategory };
}