#include "con_user_item.h"
#include "ui_con_user_item.h"
#include "red_dot_label.h"

ConUserItem::ConUserItem(QWidget *parent) :
    ListItemBase(parent),
    ui(new Ui::ConUserItem),
    _redDotLabel(nullptr)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CONTACT_USER_ITEM);
//    ui->red_point->raise();

    // 创建红点标签
    _redDotLabel = new RedDotLabel(this);
    _redDotLabel->hide();  // 默认隐藏
    // 将红点移到最上层
    _redDotLabel->raise();

    updateRedDotPosition();
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}

void ConUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
{
    _info = std::make_shared<UserInfo>(auth_info);
    // 加载图片
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
     _info = std::make_shared<UserInfo>(uid,name, name, icon, 0);

     // 加载图片
     QPixmap pixmap(_info->_icon);

     // 设置图片自动缩放
     ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
     ui->icon_lb->setScaledContents(true);

     ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::SetRedDot(bool show, int count)
{
    _redDotLabel->setCount(show, count);
    _redDotLabel->show();
    _redDotLabel->raise();  // 确保在最上层
}

void ConUserItem::SetInfo(std::shared_ptr<AuthRsp> auth_rsp){
    _info = std::make_shared<UserInfo>(auth_rsp);

    // 加载图片
    QPixmap pixmap(_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_info->_name);
}

std::shared_ptr<UserInfo> ConUserItem::GetInfo()
{
    return _info;
}

void ConUserItem::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateRedDotPosition();
}

void ConUserItem::updateRedDotPosition()
{
    if (!ui->icon_lb->geometry().isValid()) {
        return;
    }

    QRect iconRect = ui->icon_lb->geometry();
    QSize dotSize = _redDotLabel->size();

    // 将红点放置在图标右上角
    int x = iconRect.right() - dotSize.width() / 2 + 8;
    int y = iconRect.top() - dotSize.height() / 2 + 5;

    _redDotLabel->move(x, y);
    _redDotLabel->raise();  // 确保在最上层
}
