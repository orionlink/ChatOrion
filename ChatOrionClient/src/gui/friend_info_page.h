#ifndef FRIEND_INFO_PAGE_H
#define FRIEND_INFO_PAGE_H

#include <QWidget>

#include "user_data.h"

namespace Ui {
class FriendInfoPage;
}

class FriendInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit FriendInfoPage(QWidget *parent = nullptr);
    ~FriendInfoPage();

    void SetInfo(std::shared_ptr<UserInfo> ui);
signals:
    void sig_jump_chat_item(std::shared_ptr<UserInfo> si);
private slots:
    void on_msg_chat_clicked();

private:
    Ui::FriendInfoPage *ui;

    std::shared_ptr<UserInfo> _user_info;
};

#endif // FRIEND_INFO_PAGE_H
