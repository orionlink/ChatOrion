#include "red_dot_label.h"

RedDotLabel::RedDotLabel(QWidget *parent) : QLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    this->setFixedSize(25, 24);  // 设置固定大小
}

void RedDotLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (_is_show)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QPoint center = rect().center();
        int radius = (_count > 0) ? 8 : 5;

        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);

//        if (_radius != 8) radius = _radius;

        if (_count >= 10)
        {
            // 绘制两个圆形做相交，即可做出药丸型，计算两个圆的中心
            int offset = 6;
            QPoint leftCircle(center.x() - radius + offset, center.y());
            QPoint rightCircle(center.x() + radius - offset, center.y());

            painter.drawEllipse(leftCircle, radius, radius);
            painter.drawEllipse(rightCircle, radius, radius);
        }
        else
        {
            // 小于10时保持圆形
            painter.drawEllipse(center, radius, radius);
        }

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
