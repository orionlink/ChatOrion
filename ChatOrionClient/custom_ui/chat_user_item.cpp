#include "chat_user_item.h"
#include "ui_chat_user_item.h"
#include "red_dot_label.h"

#include <QPainter>

ChatUserItem::ChatUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ChatUserItem),
    redDotLabel(nullptr)
{
    ui->setupUi(this);

    // 创建红点标签
    redDotLabel = new RedDotLabel(this);
    redDotLabel->setFixedSize(24, 24);  // 设置固定大小
    redDotLabel->hide();  // 默认隐藏
    // 将红点移到最上层
    redDotLabel->raise();
}

ChatUserItem::~ChatUserItem()
{
    delete ui;
}

void ChatUserItem::SetInfo(QString name, QString head, QString msg)
{
    _name = name;
    _head = head;
    _msg = msg;
    // 加载图片
    QPixmap pixmap(_head);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_name);
    ui->user_chat_lb->setText(_msg);

    updateRedDotPosition();
}

void ChatUserItem::SetRedDot(bool show, int count)
{
    redDotLabel->setCount(show, count);
    redDotLabel->show();
    redDotLabel->raise();  // 确保在最上层
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
