#include "apply_friend_dialog.h"
#include "ui_apply_friend_dialog.h"
#include "user_mgr.h"
#include "tcp_mgr.h"

#include <QScrollBar>

ApplyFriend::ApplyFriend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyFriendDialog)
{
    ui->setupUi(this);
}

ApplyFriend::~ApplyFriend()
{
    qDebug()<< "ApplyFriend destruct";
    delete ui;
}
