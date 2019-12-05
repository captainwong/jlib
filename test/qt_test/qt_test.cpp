#include "qt_test.h"
#include <qlayout.h>

#include "../../jlib/log2.h"
#include "qt/ErrorCode.h"
#include "qt/QtDebug.h"
#include "qt/QtDebugOutput.h"
#include "qt/QtPathHelper.h"
#include "qt/QtStylesheet.h"
#include "qt/QtUtils.h"
#include "qt/Ctrl/ThreadCtrl.h"
#include "qt/Model/ThreadModel.h"
#include "qt/View/BgColorBtn.h"
#include "qt/View/HttpDlg.h"
#include "qt/View/HttpDlgErrorCode.h"
#include "qt/View/IconBtn.h"
#include "qt/View/LoadingView.h"
#include "qt/View/PageTitle.h"
#include "qt/View/RndButton.h"

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
