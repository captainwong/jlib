#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qt_test.h"

class qt_test : public QMainWindow
{
	Q_OBJECT

public:
	qt_test(QWidget *parent = Q_NULLPTR);

private:
	Ui::test_qtClass ui;
};
