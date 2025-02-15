//
// Created by hwk on 2025/1/7.
//

#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include "MySQLConnectPool.h"
#include "const.h"
#include "data.h"
#include "ChatCacheManager.h"

class MySQLDao
{
public:
    MySQLDao();
    ~MySQLDao();

    bool init();

    std::shared_ptr<UserInfo> GetUser(int uid);
    std::shared_ptr<UserInfo> GetUser(std::string name);
    bool AddFriendApply(const int& from_uid, const int& to_uid);

    bool AuthFriendApply(const int& from_uid, const int& to_uid);

    bool AddFriend(const int& from_uid, const int& to_uid, std::string back_name);

    bool GetApplyList(int self_id, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int offset, int limit);

    bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info_list);

    bool MarkMessagesAsRead(int uid, int peer_id);

    bool SaveChatMessage(int from_uid, int to_uid, const std::string& msg_id,
            const std::string& content, int msg_type = 1);

    std::vector<ChatMessage> GetRecentMessages(int uid, int limit = 50, int64_t before_id = 0);

    // 更新会话最后一条消息
    bool UpdateLastMessage(int uid, int peerId, const std::string& msgId);
private:
    // 拆分 SQL 脚本为多个语句
    std::vector<std::string> splitSQLScript(const std::string& sql_content);

    std::unique_ptr<MySQLConnectPool> _pool;
};



#endif //MYSQLDAO_H
