#include "self_info_dialog.h"
#include "ui_self_info_dialog.h"

#include <QPainter>
#include <QColor>

SelfInfoDialog::SelfInfoDialog(QWidget *parent) :
    BaseDialog(parent),
    ui(new Ui::SelfInfoDialog),
    _user_info(nullptr)
{
    ui->setupUi(this);
}

SelfInfoDialog::~SelfInfoDialog()
{
    delete ui;
}

void SelfInfoDialog::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    if (user_info == nullptr) return;

    _user_info = user_info;

#if 1
    _user_info->_icon = ":/res/pic/head_1.jpg";
#endif
    // 加载图片
    QPixmap pixmap(_user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->name_lb->setText(user_info->_name);
    ui->nick_lb->setText(user_info->_name);
    ui->uid_lb->setText(QString("%1").arg(user_info->_uid));

    int sex = user_info->_sex;
    QPixmap pixmapSex;
    if (sex)
    {
        pixmapSex.load(QStringLiteral(":/res/pic/用户-男.png"));
        ui->sex_lb->setPixmap(pixmapSex.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        pixmapSex.load(QStringLiteral(":/res/pic/用户-女.png"));
        ui->sex_lb->setPixmap(pixmapSex.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    ui->sex_lb->setScaledContents(true);
}
