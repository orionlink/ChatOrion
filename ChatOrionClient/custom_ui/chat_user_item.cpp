#include "chat_user_item.h"
#include "ui_chat_user_item.h"
#include "red_dot_label.h"
#include "tools.h"

#include <QPainter>
#include <QDateTime>
#include <QDebug>

ChatUserItem::ChatUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ChatUserItem),
    redDotLabel(nullptr)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CHAT_USER_ITEM);
    // 创建红点标签
    redDotLabel = new RedDotLabel(this);
    redDotLabel->hide();  // 默认隐藏
    // 将红点移到最上层
    redDotLabel->raise();
}

ChatUserItem::~ChatUserItem()
{
    delete ui;
}

void ChatUserItem::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    // 加载图片
    QPixmap pixmap(_user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_user_info->_name);
    ui->user_chat_lb->setText(_user_info->_last_msg);

    QDateTime datetime = QDateTime::fromSecsSinceEpoch(_user_info->_last_msg_time);
    qDebug() << "datetime: " << datetime;
    ui->time_lb->setText(Tools::getFormattedTimeString(datetime));
    qDebug() << "datetime-str: " << Tools::getFormattedTimeString(datetime);
    ui->time_lb->setVisible(true);
}

void ChatUserItem::SetInfo(std::shared_ptr<FriendInfo> friend_info)
{
    if (friend_info == nullptr) return;

    _user_info = std::make_shared<UserInfo>(friend_info);
    // 加载图片
    QPixmap pixmap(_user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_user_info->_name);
    ui->user_chat_lb->setText(_user_info->_last_msg);

    QDateTime datetime = QDateTime::fromSecsSinceEpoch(_user_info->_last_msg_time);
    ui->time_lb->setText(Tools::getFormattedTimeString(datetime));
    ui->time_lb->setVisible(true);
}

void ChatUserItem::SetInfo(QString name, QString head, QString msg, int64_t last_msg_time, QString last_msg)
{
    // 加载图片
    QPixmap pixmap(head);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(name);
    ui->user_chat_lb->setText(msg);
    ui->user_chat_lb->setText(last_msg);

    QDateTime datetime = QDateTime::fromSecsSinceEpoch(last_msg_time);
    ui->time_lb->setText(Tools::getFormattedTimeString(datetime));
    ui->time_lb->setVisible(true);
}

void ChatUserItem::SetRedDot(bool show, int count)
{
    redDotLabel->setCount(show, count);
    redDotLabel->show();
    redDotLabel->raise();  // 确保在最上层
}

std::shared_ptr<UserInfo> ChatUserItem::GetUserInfo()
{
    return _user_info;
}

void ChatUserItem::updateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{
    QString last_msg = "";
    int64_t last_msg_time = 0;
    for (auto& msg : msgs) {
        last_msg = msg->_msg_content;
        last_msg_time = msg->_send_time;
        _user_info->_chat_msgs.push_back(msg);
    }

    _user_info->_last_msg = last_msg;
    _user_info->_last_msg_time = last_msg_time;

    ui->user_chat_lb->setText(_user_info->_last_msg);
    QDateTime datetime = QDateTime::fromSecsSinceEpoch(_user_info->_last_msg_time);
    ui->time_lb->setText(Tools::getFormattedTimeString(datetime));
}

void ChatUserItem::SetLastMsg(const QString &msg)
{
    ui->user_chat_lb->setText(msg);
}

void ChatUserItem::SetLastMsgTime(int64_t time)
{
    QDateTime datetime = QDateTime::fromSecsSinceEpoch(time);
    ui->time_lb->setText(Tools::getFormattedTimeString(datetime));
}

void ChatUserItem::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateRedDotPosition();
}

void ChatUserItem::updateRedDotPosition()
{
    if (!ui->icon_lb->geometry().isValid()) {
        return;
    }

    QRect iconRect = ui->icon_lb->geometry();
    QSize dotSize = redDotLabel->size();

    // 将红点放置在图标右上角
    int x = iconRect.right() - dotSize.width() / 2;
    int y = iconRect.top() - dotSize.height() / 2;

    redDotLabel->move(x, y);
    redDotLabel->raise();  // 确保在最上层
}
