#pragma once
#include <QtWidgets>

namespace HBVideoPlatform {
namespace common {

class Toast : public QLabel
{
	Q_OBJECT

protected:
	Toast(QWidget* parent = nullptr);

public:
	enum ToastLength {
		LENGTH_SHORT = 3,
		LENGTH_LONG = 5,
	};

	static void makeToast(QWidget* context, const QString& msg, ToastLength length = LENGTH_SHORT);

protected:
	void buildMsg(const QString& msg);
	void timerEvent(QTimerEvent *e);

private:
	int timerId = -1;
	int length_ = LENGTH_SHORT;
	//QLabel* label_ = {};
};


}
}
