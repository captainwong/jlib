#pragma once

#include <QScrollArea>

class BaseScrollView : public QScrollArea
{
	Q_OBJECT

public:
	BaseScrollView(QWidget *parent = nullptr);
	~BaseScrollView();

	auto root() const { return root_; }
	virtual void refresh();

protected:
	virtual void resizeEvent(QResizeEvent*) override;	

private:
	QWidget* root_ = {};
};
