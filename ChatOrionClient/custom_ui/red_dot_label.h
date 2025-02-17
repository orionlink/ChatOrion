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

//    void set_radius(int radius)
//    {
//        if (radius <= 0) return;  // 防止无效值
//        _radius = radius;
//        // 根据新的半径更新控件大小
//        setFixedSize(_radius * 2, _radius * 2);
//        update();
//    }
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int _count{0};
    bool _is_show{false};
//    int _radius;
};

#endif // REDDOTLABEL_H
