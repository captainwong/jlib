#pragma once

#include <QDialog>
#include <QPixmap>

class QLabel;

class QrCodeView : public QDialog
{
	Q_OBJECT
public:
	QrCodeView(QWidget* parent, const QString& title, const QString& content, const QSize size = QSize(400, 400));
	QrCodeView(QWidget* parent, const QString& title, const QPixmap& pixmap, const QSize size = QSize(400, 400));

	void setContent(const QString& content);
	void setPixmap(const QPixmap& pixmap);

protected:
	void create(const QString& title, QSize size);

private:
	QLabel* label{};

};
