#ifndef BARRULER_H
#define BARRULER_H

#include <QWidget>
#include "../../../util/micro_getter_setter.h"

class BarRuler : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue)
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue)
    Q_PROPERTY(double value READ getValue WRITE setValue)
    Q_PROPERTY(int precision READ getPrecision WRITE setPrecision)

    Q_PROPERTY(int longStep READ getLongStep WRITE setLongStep)
    Q_PROPERTY(int shortStep READ getShortStep WRITE setShortStep)
    Q_PROPERTY(int space READ getSpace WRITE setSpace)

    Q_PROPERTY(bool animation READ getAnimation WRITE setAnimation)
    Q_PROPERTY(double animationStep READ getAnimationStep WRITE setAnimationStep)

    Q_PROPERTY(QColor bgColorStart READ getBgColorStart WRITE setBgColorStart)
    Q_PROPERTY(QColor bgColorEnd READ getBgColorEnd WRITE setBgColorEnd)
    Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor)
    Q_PROPERTY(QColor barBgColor READ getBarBgColor WRITE setBarBgColor)
    Q_PROPERTY(QColor barColor READ getBarColor WRITE setBarColor)

    //! 最小值
    DECLARE_PRI_MDI_PUB_G(double, minValue, getMinValue)
    //! 最大值
    DECLARE_PRI_MDI_PUB_G(double, maxValue, getMaxValue)
    //! 目标值
    DECLARE_PRI_MDI_PUB_G(double, value, getValue)
    //! 精度，小数点后几位
    DECLARE_PRI_MDI_PUB_G(int, precision, getPrecision)

    //! 长线条等分步长
    DECLARE_PRI_MDI_PUB_G(int, longStep, getLongStep)
    //! 短线条等分步长
    DECLARE_PRI_MDI_PUB_G(int, shortStep, getShortStep)
    //! 间距
    DECLARE_PRI_MDI_PUB_G(int, space, getSpace)

    //! 动画效果
    DECLARE_PRI_MDI_PUB_G(bool, animation, getAnimation)
    //! 动画步长
    DECLARE_PRI_MDI_PUB_G(double, animationStep, getAnimationStep)

    //! 背景渐变开始
    DECLARE_PRI_MDI_PUB_G(QColor, bgColorStart, getBgColorStart)
    //! 背景渐变结束
    DECLARE_PRI_MDI_PUB_G(QColor, bgColorEnd, getBgColorEnd)
    //! 线条颜色
    DECLARE_PRI_MDI_PUB_G(QColor, lineColor, getLineColor)
    //! 柱背景色
    DECLARE_PRI_MDI_PUB_G(QColor, barBgColor, getBarBgColor)
    //! 柱颜色
    DECLARE_PRI_MDI_PUB_G(QColor, barColor, getBarColor)

public:
    explicit BarRuler(QWidget *parent = 0);

signals:
    void valueChanged(double value);

public slots:
    //! 设置最大最小值-范围值
    void setRange(double minValue, double maxValue);
    void setRange(int minValue, int maxValue);

    //! 设置最大最小值
    void setMinValue(double minValue);
    void setMaxValue(double maxValue);

    //! 设置目标值
    void setValue(double value);
    void setValue(int value);

    //! 设置精确度
    void setPrecision(int precision);
    //! 设置线条等分步长
    void setLongStep(int longStep);
    void setShortStep(int shortStep);
    //! 设置间距
    void setSpace(int space);

    //! 设置是否启用动画显示
    void setAnimation(bool animation);
    //! 设置动画显示的步长
    void setAnimationStep(double animationStep);

    //! 设置背景颜色
    void setBgColorStart(const QColor &bgColorStart);
    void setBgColorEnd(const QColor &bgColorEnd);
    //! 设置线条颜色
    void setLineColor(const QColor &lineColor);
    //! 设置柱状颜色
    void setBarBgColor(const QColor &barBgColor);
    void setBarColor(const QColor &barColor);


protected:
    void paintEvent(QPaintEvent *);
    void drawBg(QPainter *painter);
    void drawRuler(QPainter *painter);
    void drawBarBg(QPainter *painter);
    void drawBar(QPainter *painter);

private slots:
    void updateValue();

private:           
    //! 是否倒退
    bool reverse;                   
    //! 当前值
    double currentValue;            
    //! 定时器绘制动画
    QTimer *timer;                  
    //! 柱状图区域
    QRectF barRect;                 

};

#endif
