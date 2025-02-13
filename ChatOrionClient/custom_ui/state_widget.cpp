#include "state_widget.h"
#include "red_dot_label.h"

#include <QVBoxLayout>
#include <QStyleOption>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

StateWidget::StateWidget(QWidget *parent): QWidget(parent),_curstate(ClickLbState::Normal), _redDotLabel(nullptr)
{
    setCursor(Qt::PointingHandCursor);
//    //添加红点
//    AddRedPoint();
    SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

    // 创建红点标签
    _redDotLabel = new RedDotLabel((QWidget*)parent->parent());
    _redDotLabel->hide();  // 默认隐藏
    // 将红点移到最上层
    _redDotLabel->raise();

    _redDotLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

void StateWidget::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;

    _selected = select;
    _selected_hover = select_hover;
    _selected_press = select_press;

    setProperty("state", normal);
    repolish(this);
}

ClickLbState StateWidget::GetCurState()
{
    return _curstate;
}

void StateWidget::ClearState()
{
    _curstate = ClickLbState::Normal;
    setProperty("state",_normal);
    repolish(this);
    update();
}

void StateWidget::SetSelected(bool bselected)
{
    if(bselected){
        _curstate = ClickLbState::Selected;
        setProperty("state",_selected);
        repolish(this);
        update();
        return;
    }

    _curstate = ClickLbState::Normal;
    setProperty("state",_normal);
    repolish(this);
    update();
    return;
}

void StateWidget::SetRedDot(bool show, int count)
{
    _redDotLabel->setCount(show, count);
    _redDotLabel->setVisible(true);
    _redDotLabel->raise();  // 确保在最上层
}

void StateWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    return;
}

void StateWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Selected){
            qDebug()<<"PressEvent , already to selected press: "<< _selected_press;
            //emit clicked();
            // 调用基类的mousePressEvent以保证正常的事件处理
            QWidget::mousePressEvent(event);
            return;
        }

        if(_curstate == ClickLbState::Normal){
            qDebug()<<"PressEvent , change to selected press: "<< _selected_press;
            _curstate = ClickLbState::Selected;
            setProperty("state",_selected_press);
            repolish(this);
            update();
        }

        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QWidget::mousePressEvent(event);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal){
            //qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",_normal_hover);
            repolish(this);
            update();

        }else{
            //qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked();
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QWidget::mousePressEvent(event);
}

void StateWidget::enterEvent(QEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(_curstate == ClickLbState::Normal){
         //qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",_normal_hover);
        repolish(this);
        update();

    }else{
         //qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_selected_hover);
        repolish(this);
        update();
    }

    QWidget::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent *event)
{
    // 在这里处理鼠标悬停离开的逻辑
    if(_curstate == ClickLbState::Normal){
        // qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
        repolish(this);
        update();

    }else{
        // qDebug()<<"leave , change to select normal : "<< _selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    QWidget::leaveEvent(event);
}

void StateWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateRedDotPosition();
}

void StateWidget::updateRedDotPosition()
{
    if (!this->geometry().isValid()) {
        return;
    }

    // 将红点放置在右上角
    int x = this->pos().x() + width() -10;
    int y = this->pos().y() + 20; // 顶部对齐

    _redDotLabel->move(x, y);
    _redDotLabel->raise();
}

