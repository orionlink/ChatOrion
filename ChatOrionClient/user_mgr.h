#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include <QMap>

#include "singleton.h"
#include "user_data.h"

class UserMgr : public QObject, public Singleton<UserMgr>, public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~UserMgr();

    void SetUserInfo(std::shared_ptr<UserInfo> user_info);

    void SetToken(QString token);

    int GetUid();

    QString GetName();

    QString GetIcon();

    std::shared_ptr<UserInfo> GetUserInfo();

    void AppendFriendList(QJsonArray array);
    bool CheckFriendById(int uid);

    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
private:
    explicit UserMgr();

    std::shared_ptr<UserInfo> _user_info;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
    QString _token;
};

#endif // USERMGR_H
