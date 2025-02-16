//
// Created by hwk on 2025/1/7.
//

#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H

#include "singleton.hpp"
#include "const.h"
#include "MySQLDao.h"
#include "data.h"

class MySQLManager : public Singleton<MySQLManager>
{
    friend class Singleton<MySQLManager>;
public:

    bool init();

    std::shared_ptr<UserInfo> GetUser(int uid);
    std::shared_ptr<UserInfo> GetUser(std::string name);

    bool AddFriendApply(const int& from_uid, const int& to_uid);

    bool AuthFriendApply(const int& from_uid, const int& to_uid);

    bool AddFriend(const int& from_uid, const int& to_uid, std::string back_name);

    bool GetApplyList(int self_id, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int offset, int limit);

    bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info);

    bool MarkMessagesAsRead(int uid, int peer_id);

    bool SaveChatMessage(int from_uid, int to_uid, const std::string& msg_id,
                const std::string& content, int msg_type = 1);

    bool SaveChatMessage(const ChatMessage& chat_message);

    std::vector<ChatMessage> GetRecentMessages(int uid, int limit = 50, int64_t before_id = 0);

    // 更新会话最后一条消息
    bool UpdateLastMessage(int uid, int peerId, const std::string& msgId);
private:
    MySQLDao _dao;
};



#endif //MYSQLMANAGER_H
