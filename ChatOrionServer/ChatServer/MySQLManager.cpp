//
// Created by hwk on 2025/1/7.
//

#include "MySQLManager.h"

bool MySQLManager::init()
{
    return _dao.init();
}

std::shared_ptr<UserInfo> MySQLManager::GetUser(int uid)
{
    return _dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MySQLManager::GetUser(std::string name)
{
    return _dao.GetUser(name);
}

bool MySQLManager::AddFriendApply(const int &from_uid, const int &to_uid)
{
    return _dao.AddFriendApply(from_uid, to_uid);
}

bool MySQLManager::AuthFriendApply(const int &from_uid, const int &to_uid)
{
    return _dao.AuthFriendApply(from_uid, to_uid);
}

bool MySQLManager::AddFriend(const int &from_uid, const int &to_uid, std::string back_name)
{
    return _dao.AddFriend(from_uid, to_uid, back_name);
}

bool MySQLManager::GetApplyList(int self_id, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int offset, int limit)
{
    return _dao.GetApplyList(self_id, applyList, offset, limit);
}

bool MySQLManager::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_info_list)
{
    return _dao.GetFriendList(self_id, user_info_list);
}

bool MySQLManager::MarkMessagesAsRead(int uid, int peer_id)
{
    return _dao.MarkMessagesAsRead(uid, peer_id);
}

bool MySQLManager::SaveChatMessage(int from_uid, int to_uid, const std::string& msg_id, const std::string& content,
    int msg_type)
{
    return _dao.SaveChatMessage(from_uid, to_uid, msg_id, content);
}

std::vector<ChatMessage> MySQLManager::GetRecentMessages(int uid, int limit, int64_t before_id)
{
    return _dao.GetRecentMessages(uid, limit, before_id);
}

bool MySQLManager::UpdateLastMessage(int uid, int peerId, const std::string& msgId)
{
    return _dao.UpdateLastMessage(uid, peerId, msgId);
}
