#include "base_dialog.h"

#include <QPainter>
#include <QMouseEvent>
#include <QBitmap>
#include <QStyleOption>

BaseDialog::BaseDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

void BaseDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿

    // 设置圆角矩形的区域
    QRectF rect = this->rect();

    // 创建绘制路径
    QPainterPath path;
    path.addRoundedRect(rect, 10, 10); // 10,10 是圆角的半径

    // 设置画笔（边框）
    painter.setPen(QPen(QColor(180, 180, 180), 1)); // 灰色边框，宽度1

    // 设置画刷（背景）
    painter.setBrush(QColor(255, 255, 255)); // 白色背景

    // 绘制圆角矩形
    painter.drawPath(path);

}

void BaseDialog::showEvent(QShowEvent *event)
{
    if (parentWidget()) {
        raise();
        activateWindow();
    }
    QDialog::showEvent(event);
}

void BaseDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        _isPressed = true;
        _dragPosition = event->globalPos() - frameGeometry().topLeft();
    }
}

void BaseDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (_isPressed) {
        move(event->globalPos() - _dragPosition);
    }
}

void BaseDialog::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    _isPressed = false;
}
