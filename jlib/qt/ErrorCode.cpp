#include "ErrorCode.h"
#include <QObject>

namespace
{

struct CommonErrorCatagory : public std::error_category
{
	const char* name() const noexcept override {
		return "Common Error";
	}

	std::string message(int ev) const override {
		switch (static_cast<JLIBQT_NAMESPACE DatabaseErrorCode>(ev)) {
		case JLIBQT_NAMESPACE DatabaseErrorCode::OpenDbFailed:
			return QObject::tr("Failed to open database").toLocal8Bit().toStdString();
		case JLIBQT_NAMESPACE DatabaseErrorCode::CreateDbTableFailed:
			return QObject::tr("Failed to create table").toLocal8Bit().toStdString();
		case JLIBQT_NAMESPACE DatabaseErrorCode::QueryDbFailed:
			return QObject::tr("Failed to query database").toLocal8Bit().toStdString();
		default:
			return QObject::tr("Unkown Error").toLocal8Bit().toStdString();
		}
	}
};

static const CommonErrorCatagory theDatabaseErrorCatagory = {};

}

std::error_code std::make_error_code(JLIBQT_NAMESPACE DatabaseErrorCode ec)
{
	return { static_cast<int>(ec), theDatabaseErrorCatagory };
}
