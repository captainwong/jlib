#include "ImageTxtBtn.h"
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include "../QtStylesheet.h"


ImageTxtBtn::ImageTxtBtn(QWidget* parent)
	: QLabel(parent)
{
	txt = new QLabel(this); 
	txt->setAttribute(Qt::WA_TranslucentBackground, true);
	txt->setStyleSheet(jlib::qt::build_style(fontColor, fontSize, this->fontFamily) + (this->bold ? "font-weight:bold;" : ""));
	txt->setAlignment(Qt::AlignCenter);
	txt->hide();
}

void ImageTxtBtn::setPixNormal(const QPixmap& pix)
{
	pixNormal = pix;
	refresh();
}

void ImageTxtBtn::setPixHover(const QPixmap& pix)
{
	pixHover = pix;
	refresh();
}

void ImageTxtBtn::setPixPressed(const QPixmap& pix)
{
	pixPressed = pix;
	refresh();
}

void ImageTxtBtn::setPixDisabled(const QPixmap& pix)
{
	pixDisabled = pix;
	refresh();  
}

void ImageTxtBtn::setText(const QString& text)
{
	txt->setText(text);
	txt->resize(size());
	txt->move(0, 0);
	txt->show();
}

void ImageTxtBtn::setTextFont(const QString& fontFamily, QColor color, int font_size)
{
	this->fontFamily = fontFamily;
	fontColor = color;
	fontSize = font_size;
	txt->setStyleSheet(jlib::qt::build_style(fontColor, fontSize, this->fontFamily) + (this->bold ? "font-weight:bold;" : ""));
}

void ImageTxtBtn::setTextFontFamily(const QString& fontFamily)
{
	this->fontFamily = fontFamily;
	txt->setStyleSheet(jlib::qt::build_style(fontColor, fontSize, this->fontFamily) + (this->bold ? "font-weight:bold;" : ""));
}

void ImageTxtBtn::setTextColor(QColor color)
{
	fontColor = color;
	txt->setStyleSheet(jlib::qt::build_style(fontColor, fontSize, this->fontFamily) + (this->bold ? "font-weight:bold;" : ""));
}

void ImageTxtBtn::setTextSize(int sz)
{
	fontSize = sz;
	txt->setStyleSheet(jlib::qt::build_style(fontColor, fontSize, this->fontFamily) + (this->bold ? "font-weight:bold;" : ""));
}

void ImageTxtBtn::setTextBold(bool bold)
{
	this->bold = bold;
	txt->setStyleSheet(jlib::qt::build_style(fontColor, fontSize, this->fontFamily) + (this->bold ? "font-weight:bold;" : ""));

}

void ImageTxtBtn::setEnabled(bool able)
{
	is_disabled_ = !able;
	__super::setEnabled(able);
	refresh();
}

void ImageTxtBtn::enterEvent(QEvent* e)
{
	if (is_disabled_) { return; }
	setCursor(QCursor(Qt::PointingHandCursor));
	is_hover_ = true;
	refresh();
}

void ImageTxtBtn::leaveEvent(QEvent* e)
{
	if (is_disabled_) { return; }
	setCursor(QCursor(Qt::ArrowCursor));
	is_hover_ = false;
	refresh();
}

void ImageTxtBtn::mousePressEvent(QMouseEvent* e)
{
	if (is_disabled_ || e->button() != Qt::LeftButton)return;
	is_pressed_ = true;
	refresh();
}

void ImageTxtBtn::mouseReleaseEvent(QMouseEvent* e)
{
	if (is_disabled_ || e->button() != Qt::LeftButton)return;
	is_pressed_ = false;
	refresh();
	emit clicked();
}

void ImageTxtBtn::refresh()
{
	QPixmap pix = pixNormal; 
	
	if (is_hover_ && !pixHover.isNull()) {
		pix = pixHover;
	}
	if (is_pressed_ && !pixPressed.isNull()) {
		pix = pixPressed;
	}
	if (is_disabled_ && !pixDisabled.isNull()) {
		pix = pixDisabled;
	}

	if (!pix.isNull()) {
		setPixmap(pix);
	}
}
