#include "RndButton.h"

#include "../QtStylesheet.h"
#include "../QtUtils.h"

using namespace jlib::qt;

RndButton::RndButton(QWidget* parent)
    : QWidget(parent) {
    txt_ = new QLabel(this);
    txt_->setAlignment(Qt::AlignCenter);
    txt_->hide();

    normal_color_ = def_colors::control_bk;
    hightlight_color_ = Qt::lightGray;
    font_sz_ = 12;
    txt_color_ = Qt::white;
}

RndButton::~RndButton() {
}

void RndButton::set_attr(const QString& txt, const QSize& sz, int font_size) {
    font_sz_ = font_size;
    txt_->setStyleSheet(build_style(Qt::white, font_size));
    txt_->setText(txt);
    setFixedSize(sz);

    txt_->resize(size());
    txt_->move(0, 0);
    txt_->show();

    bk_color_ = def_colors::control_bk;

    update();
}

void RndButton::set_text(const QString& txt) {
    txt_->setText(txt);
}

void RndButton::set_size(const QSize& sz) {
    setFixedSize(sz);
    txt_->resize(size());
    txt_->move(0, 0);
}

void RndButton::set_font_size(int font_size) {
    font_sz_ = font_size;
    txt_->setStyleSheet(build_style(txt_color_, font_sz_));
}

void RndButton::set_highlight(bool on) {
    is_highlighted_ = on;
    bk_color_ = is_highlighted_ ? hightlight_color_ : normal_color_;
    update();
}

void RndButton::set_normal_color(QColor color) {
    normal_color_ = color;
    update();
}

void RndButton::set_highlight_color(QColor color) {
    hightlight_color_ = color;
    update();
}

void RndButton::set_text_color(QColor color) {
    txt_color_ = color;
    txt_->setStyleSheet(build_style(txt_color_, font_sz_));
}

void RndButton::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path;
    path.addRoundedRect(QRectF(0, 0, width(), height()), 5, 5);
    QPen pen(Qt::black, 1);
    painter.setPen(pen);
    painter.fillPath(path, bk_color_);
    painter.drawPath(path);
}

void RndButton::enterEvent(QEvent* e) {
    if (!isEnabled()) {
        return;
    }
    setCursor(QCursor(Qt::PointingHandCursor));
    bk_color_ = normal_color_;
    update();

    emit sig_focus_on();
}

void RndButton::leaveEvent(QEvent* e) {
    if (!isEnabled()) {
        return;
    }
    setCursor(QCursor(Qt::ArrowCursor));

    bk_color_ = is_highlighted_ ? hightlight_color_ : normal_color_;
    update();

    is_pressed_ = false;
}

void RndButton::mousePressEvent(QMouseEvent* e) {
    if (e->button() != Qt::LeftButton) return;
    bk_color_ = hightlight_color_;
    update();

    is_pressed_ = true;
}

void RndButton::mouseReleaseEvent(QMouseEvent* e) {
    if (e->button() != Qt::LeftButton) return;
    bk_color_ = normal_color_;
    update();

    if (is_pressed_) {
        MYQDEBUG << "RndButton emit clicked";
        emit clicked();
        is_pressed_ = false;
    }
}
