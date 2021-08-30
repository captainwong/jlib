#include "QrCodeView.h"
#include <QLabel>
#include <QPainter>
#include "../Util/qrcode/QrCode.hpp"

static void paintQR(QPainter& painter, const QSize sz, const QString& data, QColor fg) 
{
	// NOTE: At this point you will use the API to get the encoding and format you want, instead of my hardcoded stuff:
	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(data.toUtf8().constData(), qrcodegen::QrCode::Ecc::LOW);
	const int s = qr.getSize() > 0 ? qr.getSize() : 1;
	const double w = sz.width();
	const double h = sz.height();
	const double aspect = w / h;
	const double size = ((aspect > 1.0) ? h : w);
	const double scale = size / (s + 2);
	// NOTE: For performance reasons my implementation only draws the foreground parts in supplied color.
	// It expects background to be prepared already (in white or whatever is preferred).
	painter.setPen(Qt::NoPen);
	painter.setBrush(fg);
	for (int y = 0; y < s; y++) {
		for (int x = 0; x < s; x++) {
			const int color = qr.getModule(x, y);  // 0 for white, 1 for black
			if (0 != color) {
				const double rx1 = (x + 1) * scale, ry1 = (y + 1) * scale;
				QRectF r(rx1, ry1, scale, scale);
				painter.drawRects(&r, 1);
			}
		}
	}
}

static QPixmap genQR(const QString& content, const QSize& size)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::white);
	QPainter painter(&pixmap);
	paintQR(painter, size, content, Qt::black);
	return pixmap;
}

QrCodeView::QrCodeView(QWidget* parent, const QString& title, const QString& content, const QSize size)
	: QDialog(parent)
{
	create(title, size);
	setContent(content);
}

QrCodeView::QrCodeView(QWidget* parent, const QString& title, const QPixmap& pixmap, const QSize size)
	: QDialog(parent)
{
	create(title, size);
	setPixmap(pixmap);
}

void QrCodeView::create(const QString& title, QSize size)
{
	setWindowTitle(title);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	if (!size.isValid()) {
		size = { 400,400 };
	}
	setFixedSize(size);

	label = new QLabel(this);
	label->resize(size);
	label->move(0, 0);
}

void QrCodeView::setContent(const QString& content)
{
	setPixmap(genQR(content, size()));
}

void QrCodeView::setPixmap(const QPixmap& pixmap)
{
	auto pix = pixmap.scaled(size());
	label->setPixmap(pix);
	update();
}
