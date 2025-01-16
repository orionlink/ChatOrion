#include "chat_user_item.h"
#include "ui_chatuser_item.h"

ChatUserItem::ChatUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ChatUserItem)
{
    ui->setupUi(this);
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
}
