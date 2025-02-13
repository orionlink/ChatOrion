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

    /**
     * @brief 获取用户信息
     * @return
     */
    std::shared_ptr<UserInfo> GetUserInfo();

    /**
     * @brief 以追加的方式添加到好友列表
     * @param array
     */
    void AppendFriendList(QJsonArray array);

    /**
     * @brief 校验该用户是否是我的好友
     * @param array
     */
    bool CheckFriendById(int uid);

    /**
     * @brief 获取请求列表
     * @return
     */
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();

    /**
     * @brief 以追加的方式添加到用户请求列表
     * @param array
     */
    void AppendApplyList(QJsonArray array);

    /**
     * @brief 添加一个到用户请求列表
     * @param array
     */
    void AddApplyList(std::shared_ptr<ApplyInfo> app);

    /**
     * @brief 检查用户是否已经加入到好友列表中了
     * @param uid
     * @return
     */
    bool AlreadyApply(int uid);
private:
    explicit UserMgr();

    std::shared_ptr<UserInfo> _user_info;  // 用户信息
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list; // 用户请求列表
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list; // 好友列表
    QString _token;
};

#endif // USERMGR_H
