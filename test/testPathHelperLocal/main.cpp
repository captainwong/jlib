#include <QtCore/QCoreApplication>
#include "../../jlib/qt/QtPathHelper.h"
#include "../../jlib/qt/QtDebug.h"

using namespace jlib::qt;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	PathHelperLocal helper;
	MYQDEBUG << ("exe =") << helper.exe();
	MYQDEBUG << ("program =") << helper.program();
	MYQDEBUG << ("bin =") << helper.bin();
	MYQDEBUG << ("data =") << helper.data();
	MYQDEBUG << ("config =") << helper.config();
	MYQDEBUG << ("db =") << helper.db();
	MYQDEBUG << ("log =") << helper.log();
	MYQDEBUG << ("dumps =") << helper.dumps();

	return a.exec();
}
