#include "red_dot_label.h"

RedDotLabel::RedDotLabel(QWidget *parent) : QLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
}

void RedDotLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (_is_show)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        int radius = (_count > 0) ? 8 : 5;

        // 绘制红点
        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(rect().center(), radius, radius);

        // 如果有数字则绘制数字
        if (_count > 0)
        {
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 8, QFont::Bold));
            QString text = _count > 99 ? "99+" : QString::number(_count);
            painter.drawText(rect(), Qt::AlignCenter, text);
        }
    }
}
