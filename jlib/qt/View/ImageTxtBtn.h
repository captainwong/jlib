#pragma once

#include <QLabel>
#include <QVariant>

class QLabel;
class ImageTxtBtn : public QLabel
{
	Q_OBJECT
public:
	ImageTxtBtn(QWidget* parent = nullptr);
	virtual ~ImageTxtBtn() {}

	void setPixNormal(const QPixmap& pix);
	void setPixHover(const QPixmap& pix);
	void setPixPressed(const QPixmap& pix);
	void setPixDisabled(const QPixmap& pix);

	void setText(const QString& text);
	void setTextFont(const QString& fontFamily, QColor color = Qt::black, int font_size = 12);
	void setTextFontFamily(const QString& fontFamily);
	void setTextColor(QColor color = Qt::black);
	void setTextSize(int sz = 12);
	void setTextBold(bool bold);
	void setEnabled(bool able);

	void setData(const QVariant& val) { data_ = val; }
	QVariant getData() const { return data_; }

signals:
	void clicked();

protected:
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void mouseReleaseEvent(QMouseEvent* e) override;
	void refresh();

private:
	QPixmap pixNormal{};
	QPixmap pixHover{};
	QPixmap pixPressed{};
	QPixmap pixDisabled{};

	QLabel* txt{};
	QString fontFamily{};
	QColor fontColor = Qt::black;
	int fontSize = 12;
	bool bold = false;

	bool is_disabled_ = false;
	bool is_pressed_ = false;
	bool is_hover_ = false;

	QVariant data_{};
};
