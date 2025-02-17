#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H

#include <QListWidget>
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>
#include <memory>
#include "user_data.h"

class ConUserItem;

/**
 * @brief 联系人列表
 */
class ContactUserList : public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget* parent = nullptr);
    void SetRedDot(bool show = false, int count = 0);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override ;
private:
    void addContactUserList();

    /**
     * @brief 好友通过通知请求
     * @param len
     * @param data
     */
    void NotifyAuthFriendReq(int len, QByteArray data);

    void AuthFriendRsp(int len, QByteArray data);
public slots:
     void slot_item_clicked(QListWidgetItem *item);
signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo> user_info);
private:
    bool _load_pending;
    ConUserItem* _add_friend_item;
    QListWidgetItem * _groupitem;
};

#endif // CONTACTUSERLIST_H
