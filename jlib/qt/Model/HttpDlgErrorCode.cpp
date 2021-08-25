#include "HttpDlgErrorCode.h"
#include <QString>
#include <QObject>

namespace {

class HttpDlgErrorCategory : public std::error_category
{
public:
	const char* name() const noexcept override {
		return "HttpClient";
	}

	std::string message(int ev) const override {
		switch (static_cast<JLIBQT_NAMESPACE HttpDlgErrorCode>(ev)) {
		case JLIBQT_NAMESPACE HttpDlgErrorCode::Timeout:
			return QObject::tr("Timeout").toLocal8Bit().toStdString();
			break;
		case JLIBQT_NAMESPACE HttpDlgErrorCode::ParseJsonError:
			return QObject::tr("Cannot Parse Json").toLocal8Bit().toStdString();
			break;
		case JLIBQT_NAMESPACE HttpDlgErrorCode::HttpStatusNeq200:
			return QObject::tr("HTTP Status Code != 200").toLocal8Bit().toStdString();
			break;
		default:
			return QObject::tr("Unkown Error").toLocal8Bit().toStdString();
			break;
		}
	}
};

const HttpDlgErrorCategory theHttpDlgErrorCategory = {};

}

std::error_code std::make_error_code(JLIBQT_NAMESPACE HttpDlgErrorCode ec)
{
	return { static_cast<int>(ec), theHttpDlgErrorCategory };
}
