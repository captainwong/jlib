#include "qt_test.h"
#include <QtWidgets/QApplication>
//#include "../../jlib/log2.h"
#include "../../jlib/qt/QtDebug.h"
#include "../../jlib/qt/monitor.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//jlib::init_logger(L"qt_test");
	jlib::qt::test_monitor();

	qt_test w;
	w.show();
	return a.exec();
}
