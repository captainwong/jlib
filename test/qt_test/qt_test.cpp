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
#include "../../jlib/qt/Model/HttpDlgErrorCode.h"
#include "../../jlib/qt/View/BgColorBtn.h"
#include "../../jlib/qt/View/HttpDlg.h"
#include "../../jlib/qt/View/IconBtn.h"
#include "../../jlib/qt/View/LoadingView.h"
#include "../../jlib/qt/View/TitleBar.h"
#include "../../jlib/qt/View/RndButton.h"
#include "../../jlib/qt/View/ImageTxtBtn.h"

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

	auto rndBtn = new RndButton();
	rndBtn->set_attr(QString::fromLocal8Bit("RndButton°´Å¥"), QSize(160, 30));
	hbox->addWidget(rndBtn);

	auto imgTxtBtn = new ImageTxtBtn();
	QPixmap pix;
	pix.load(":/test_qt/image.png");
	imgTxtBtn->setPixNormal(pix);
	imgTxtBtn->setFixedSize(pix.size());
	imgTxtBtn->setText(QString::fromLocal8Bit("ImageTxtBtn°´Å¥"));
	imgTxtBtn->setTextColor(Qt::white);
	imgTxtBtn->setTextBold(true);
	hbox->addWidget(imgTxtBtn);

}
