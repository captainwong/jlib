#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_test_qt.h"

class test_qt : public QMainWindow
{
	Q_OBJECT

public:
	test_qt(QWidget *parent = Q_NULLPTR);

private:
	Ui::test_qtClass ui;
};
