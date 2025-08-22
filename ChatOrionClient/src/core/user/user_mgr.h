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

    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    std::shared_ptr<FriendInfo> GetFriendById(int uid);

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

    /**
     * @brief 通过uid返回请求信息
     * @param uid
     * @return
     */
    std::shared_ptr<ApplyInfo> GetApplyInfoByUid(int uid);

    /**
     * @brief 获取好友列表
     * @return
     */
    std::vector<std::shared_ptr<FriendInfo>> GetConListPerPage();

    /**
     * @brief 更新 _contact_loaded 变量的计数
     */
    void UpdateContactLoadedCount();

    /**
     * @brief 是否加载完联系人
     * @return
     */
    bool IsLoadConFinal();

    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();

    /**
     * @brief 是否加载聊天列表
     * @return
     */
    bool IsLoadChatFinal();
    void UpdateChatLoadedCount();

    /**
     * @brief 将好友发送的消息存储在 _friend_map 中
     * @param friend_id
     */
    void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData>>);

    /**
     * @brief 将自己发送的消息（包含服务器返回的）存储在 _user_info 中
     * @param msgs
     */
    void AppendSlefChatMsg(std::vector<std::shared_ptr<TextChatData>> msgs);
private:
    explicit UserMgr();

    std::shared_ptr<UserInfo> _user_info;  // 用户信息
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list; // 用户请求列表
    QMap<int, std::shared_ptr<ApplyInfo>> _apply_map;
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
    std::vector<std::shared_ptr<FriendInfo>> _friend_list; // 好友列表
    QString _token;

    size_t _contact_loaded;
    size_t _chat_loaded;
    const size_t _chat_count_per_page; // 每页加载的数量 默认13
};

#endif // USERMGR_H
