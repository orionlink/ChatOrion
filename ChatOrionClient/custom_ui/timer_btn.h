#ifndef TIMERBTN_H
#define TIMERBTN_H

#include <QPushButton>
#include <QTimer>

class TimerBtn : public QPushButton
{
    Q_OBJECT
public:
    explicit TimerBtn(QWidget *parent = nullptr, int time_count = 10);
protected:
     virtual void mouseReleaseEvent(QMouseEvent *e) override;
private:
    QTimer *_timer;
    int _time_count;
};

#endif // TIMERBTN_H
