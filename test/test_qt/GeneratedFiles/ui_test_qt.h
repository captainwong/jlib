/********************************************************************************
** Form generated from reading UI file 'test_qt.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST_QT_H
#define UI_TEST_QT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_test_qtClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *test_qtClass)
    {
        if (test_qtClass->objectName().isEmpty())
            test_qtClass->setObjectName(QStringLiteral("test_qtClass"));
        test_qtClass->resize(600, 400);
        menuBar = new QMenuBar(test_qtClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        test_qtClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(test_qtClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        test_qtClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(test_qtClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        test_qtClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(test_qtClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        test_qtClass->setStatusBar(statusBar);

        retranslateUi(test_qtClass);

        QMetaObject::connectSlotsByName(test_qtClass);
    } // setupUi

    void retranslateUi(QMainWindow *test_qtClass)
    {
        test_qtClass->setWindowTitle(QApplication::translate("test_qtClass", "test_qt", nullptr));
    } // retranslateUi

};

namespace Ui {
    class test_qtClass: public Ui_test_qtClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST_QT_H
