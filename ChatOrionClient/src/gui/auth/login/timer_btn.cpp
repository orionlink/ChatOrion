#include "timer_btn.h"

#include <QMouseEvent>

TimerBtn::TimerBtn(QWidget *parent, int time_count) : QPushButton(parent), _time_count(time_count)
{
    this->setStyleSheet("QPushButton{ "
                        "background-color: transparent;"
                        "border: none;"
                        "color: rgb(255, 85, 0);}");

    _timer = new QTimer(this);
    QObject::connect(_timer, &QTimer::timeout, [this, time_count](){
        _time_count--;
        if (_time_count == 0)
        {
            this->setText("获取");
            this->setEnabled(true);
            _timer->stop();
            _time_count = time_count;
            return;
        }

        setText(QString::number(_time_count));
    });
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        _timer->start(1000);
        this->setEnabled(false);
        setText(QString::number(_time_count));
        emit clicked();
    }

    QPushButton::mouseReleaseEvent(e);
}
