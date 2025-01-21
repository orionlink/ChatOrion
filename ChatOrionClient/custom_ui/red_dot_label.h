#ifndef REDDOTLABEL_H
#define REDDOTLABEL_H


#include <QPainter>
#include <QLabel>

/**
 * @brief 红点标签，用于显示消息数量等
 */
class RedDotLabel : public QLabel
{
public:
    explicit RedDotLabel(QWidget *parent = nullptr);

    void setCount(bool is_show, int count)
    {
        _count = count;
        _is_show = is_show;
        update();
    }
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int _count{0};
    bool _is_show{false};
};

#endif // REDDOTLABEL_H
