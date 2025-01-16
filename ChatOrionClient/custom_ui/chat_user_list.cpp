#include "chat_user_list.h"

ChatUserList::ChatUserList(QWidget *parent):QListWidget(parent), _load_pending(false)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this->viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        // 返回垂直方向的滚动角度增量（以 1/8 度为单位）。除以 8 是为了将角度增量转换为度数
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        // 通常，鼠标滚轮每滚动 15 度为一个步数（即一个“刻度”）。
        int numSteps = numDegrees / 15; // 计算滚动步数

        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量

        if (maxScrollValue - currentValue <= 0) {
            // 滚动到底部，加载新的联系人
//            qDebug()<<"load more chat user";
            //发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_chat_user();
         }

        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);
}
