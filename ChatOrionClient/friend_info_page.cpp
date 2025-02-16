#include "friend_info_page.h"
#include "ui_friend_info_page.h"

FriendInfoPage::FriendInfoPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendInfoPage)
{
    ui->setupUi(this);

    ui->msg_chat->SetState("normal","hover","press");
    ui->video_chat->SetState("normal","hover","press");
    ui->voice_chat->SetState("normal","hover","press");
}

FriendInfoPage::~FriendInfoPage()
{
    delete ui;
}

void FriendInfoPage::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    // 加载图片
    QPixmap pixmap(user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->name_lb->setText(user_info->_name);
    ui->nick_lb->setText(user_info->_nick);
    ui->bak_lb->setText(user_info->_nick);
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

void FriendInfoPage::on_msg_chat_clicked()
{
    emit sig_jump_chat_item(_user_info);
}
