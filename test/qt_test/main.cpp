#include "qt_test.h"
#include <QtWidgets/QApplication>
//#include "../../jlib/log2.h"
#include "../../jlib/qt/QtDebug.h"
#include "../../jlib/qt/monitor.h"
#include "../../jlib/qt/View/HttpDlg.h"

using namespace jlib::qt;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//jlib::init_logger(L"qt_test");
	//jlib::qt::test_monitor();

	//qt_test w;
	//w.show();

	Q_INIT_RESOURCE(jlibqt);

	HttpDlg dlg(nullptr, 10);

	QString url = "https://local.hb3344.com/api/test/version";
	dlg.get(url);

	return a.exec();
}
