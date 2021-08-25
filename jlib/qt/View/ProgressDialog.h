#pragma once

#include <QDialog>
#include <QElapsedTimer>
#include <QTimerEvent>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
	Q_OBJECT

public:
	ProgressDialog(QWidget *parent = Q_NULLPTR, bool enable_close = true);
	~ProgressDialog();

	// default 1000
	void setMaximum(int val);

	// caller call this to update progress or close dialog
signals:
	void sig_progress(int pos);
	void sig_done();

protected:
	virtual void timerEvent(QTimerEvent* e) override;

protected slots:
	void slot_progress(int pos);
	void slot_done();

private:
	Ui::ProgressDialog* ui{};
	QElapsedTimer timer{};
};
