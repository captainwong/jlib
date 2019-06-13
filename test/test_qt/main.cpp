#include "test_qt.h"
#include <QtWidgets/QApplication>

#include "../../jlib/log2.h"
#include "../../jlib/qt/ErrorCode.h"
#include "../../jlib/qt/QtDebug.h"
#include "../../jlib/qt/QtDebugOutput.h"
#include "../../jlib/qt/QtPathHelper.h"
#include "../../jlib/qt/QtStylesheet.h"
#include "../../jlib/qt/QtUtils.h"
#include "../../jlib/qt/Ctrl/ThreadCtrl.h"
#include "../../jlib/qt/Model/ThreadModel.h"
#include "../../jlib/qt/View/BgColorBtn.h"
#include "../../jlib/qt/View/HttpDlg.h"
#include "../../jlib/qt/View/HttpDlgErrorCode.h"
#include "../../jlib/qt/View/IconBtn.h"
#include "../../jlib/qt/View/LoadingView.h"
#include "../../jlib/qt/View/PageTitle.h"
#include "../../jlib/qt/View/RndButton.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	test_qt w;
	w.show();
	return a.exec();
}
