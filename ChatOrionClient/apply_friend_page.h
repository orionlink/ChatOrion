#ifndef APPLY_FRIEND_PAGE_H
#define APPLY_FRIEND_PAGE_H

#include <QWidget>

#include "user_data.h"

namespace Ui {
class ApplyFriendPage;
}

class ApplyFriendItem;

/**
 * @brief 好友申请页
 */
class ApplyFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();
    void AddNewApply(std::shared_ptr<AddFriendApply> apply);
protected:
    void paintEvent(QPaintEvent *event);
private:
    void loadApplyList();
    Ui::ApplyFriendPage *ui;
    std::unordered_map<int, ApplyFriendItem*> _unauth_items;
public slots:
    void slot_auth_rsp(std::shared_ptr<AuthRsp> );
signals:
    void sig_show_search(bool);
};

#endif // APPLY_FRIEND_PAGE_H
