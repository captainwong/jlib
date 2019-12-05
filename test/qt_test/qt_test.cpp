#include "qt_test.h"
#include <qlayout.h>

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

using namespace jlib::qt;

qt_test::qt_test(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	auto hbox = new QHBoxLayout();
	ui.centralWidget->setLayout(hbox);

	auto bgColorBtn = new BgColorBtn();
	bgColorBtn->setText(QString::fromLocal8Bit("BgColorBtn°´Å¥"));
	bgColorBtn->set_btn_attr(def_colors::control_bk, def_colors::control_bk_suspend, def_colors::control_text_font, 12);
	hbox->addWidget(bgColorBtn);
}
