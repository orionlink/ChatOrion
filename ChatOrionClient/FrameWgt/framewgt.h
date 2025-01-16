#ifndef FRAMEWGT_H
#define FRAMEWGT_H

#include <QWidget>
#include <QGridLayout>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>

#include "titlebar.h"

class FrameWgt : public QWidget
{
    Q_OBJECT
public:
    explicit FrameWgt(QWidget *centerWidget);
    ~FrameWgt();

    // 设置标题栏背景色
    void setTitleBarColor(const QColor &color);
    // 设置标题文字颜色
    void setTitleTextColor(const QColor &color);
    // 设置标题文字字体
    void setTitleTextFont(const QFont &font);
    // 设置标题
    void setTitleText(const QString &text);
    // 设置标题栏图标
    void setTitleIcon(const QString &path);
    // 设置标题栏图标
    void setTitleIcon(const QPixmap &icon);

    // 设置窗口颜色
    void setBackgroundColor(const QColor &color);

    // 设置边框圆角半径
    void setRadius(const uint &r);
    // 设置边框阴影颜色
    void setShadowColor(const QColor &color);
    // 设置边框阴影范围
    void setBlurRadius(const uint &r);

    // 设置隐藏标题栏最小化按扭
    void setHiddenMin(const bool &is);
    // 设置隐藏标题栏最大化按扭
    void setHiddenMax(const bool &is);
    // 设置隐藏标题栏
    void setHiddenTitleBar(const bool &is);
    // 设置标题栏高度
    void setTitleBarHeight(const uint &h);
    // 设置标题栏最小化图标
    void setMinIcon(const QIcon &icon);
    // 设置标题栏最大化图标
    void setMaxIcon(const QIcon &icon);
    // 设置标题栏关闭图标
    void setCloseIcon(const QIcon &icon);
    // 设置标题栏最小化按钮鼠标悬浮时背景色
    void setHoverColorMin(const QColor &color);
    // 设置标题栏最大化按钮鼠标悬浮时背景色
    void setHoverColorMax(const QColor &color);
    // 设置标题栏关闭按钮鼠标悬浮时背景色
    void setHoverColorClose(const QColor &color);
    // 设置标题栏最小化按钮鼠标按下时背景色
    void setPressedColorMin(const QColor &color);
    // 设置标题栏最大化按钮鼠标按下时背景色
    void setPressedColorMax(const QColor &color);
    // 设置标题栏关闭按钮鼠标按下时背景色
    void setPressedColorClose(const QColor &color);

    // 向标题后面添加控件
    void addWidgetToTitleBar(QWidget *w);
    // 向标题后面添加控件
    void addLayoutToTitleBar(QLayout *layout);

protected:
    enum OpFlag
    {
        NONE = -1,
        Top,
        Bottom,
        Left,
        Right,
        TL,
        TR,
        BL,
        BR
    };
    void initialize();
    void calculateOpflag(QPoint pos);
    void updateRadius(const uint &r);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    QWidget *m_pCenter_widget = nullptr; // 用户所提供的中间窗口
    QWidget *m_pBorder = nullptr; // 模拟边框的窗口
    TitleBar *m_pTitleBar = nullptr; // 标题栏
    QGridLayout *m_pGridLayout = nullptr;

    int m_opFlag = OpFlag::NONE; // 窗口移动、拉伸控制
    bool m_isOp = false; // 是否能移动、拉伸
    QPointF m_lastPos; // 上一次的鼠标位置
    QColor m_shadowColor = QColor(0, 0, 0, 0); // 边框阴影颜色
    int m_blurRadius = 10; // 边框阴影范围
    int m_radius = 10; // 边框圆角半径
    QColor m_bgColor = QColor(0, 0, 0, 0);
};

#endif // FRAMEWGT_H
