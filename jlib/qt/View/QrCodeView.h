#pragma once

#include <QDialog>
#include <QPixmap>

class QLabel;

class QrCodeView : public QDialog
{
	Q_OBJECT
public:
	// scale is the qrcode size/widget size ratio, 0.1 ~1
	QrCodeView(QWidget* parent, const QString& title, const QString& content, const QSize size = QSize(400, 400), float scale = 1.0f);
	QrCodeView(QWidget* parent, const QString& title, const QPixmap& pixmap, const QSize size = QSize(400, 400), float scale = 1.0f);

	void setContent(const QString& content);
	void setPixmap(const QPixmap& pixmap);

protected:
	void create(const QString& title, QSize size, float scale);

private:
	QLabel* label{};

};
