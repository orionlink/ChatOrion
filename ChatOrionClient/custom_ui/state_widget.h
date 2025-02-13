#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include <QWidget>
#include <QString>

#include "clicked_label.h"

class RedDotLabel;

/**
 * @brief 侧边栏支持点击的按钮，为了加红点显示(消息未读数量)
 */
class StateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget *parent = nullptr);

    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");

    ClickLbState GetCurState();
    void ClearState();

    void SetSelected(bool bselected);
//    void AddRedPoint();
//    void ShowRedPoint(bool show=true);
    void SetRedDot(bool show = false, int count = 0);
protected:
    void paintEvent(QPaintEvent* event) override;

    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;

    void updateRedDotPosition();
private:

    QString _normal;
    QString _normal_hover;
    QString _normal_press;

    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickLbState _curstate;
    RedDotLabel* _redDotLabel;
signals:
    void clicked(void);
};

#endif // STATEWIDGET_H
