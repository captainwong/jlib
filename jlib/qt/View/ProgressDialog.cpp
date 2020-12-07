#include "ProgressDialog.h"
#include "ui_ProgressDialog.h"

ProgressDialog::ProgressDialog(QWidget *parent, bool enable_close)
	: QDialog(parent)
{
	ui = new Ui::ProgressDialog();
	ui->setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	if (!enable_close) {
		setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
	}
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);

	connect(this, &ProgressDialog::sig_progress, this, &ProgressDialog::slot_progress, Qt::QueuedConnection);
	connect(this, &ProgressDialog::sig_done, this, &ProgressDialog::slot_done, Qt::QueuedConnection);

	timer.start();
	startTimer(500);
}

ProgressDialog::~ProgressDialog()
{
	delete ui;
}

void ProgressDialog::timerEvent(QTimerEvent* e)
{
	auto secs = timer.elapsed() / 1000;
	char txt[1024];
	sprintf(txt, "%02d:%02d", int(secs / 60), int(secs % 60));
	ui->labelTime->setText(txt);
}

void ProgressDialog::slot_progress(int pos)
{
	ui->progressBar->setValue(pos);
}

void ProgressDialog::slot_done()
{
	accept();
}
