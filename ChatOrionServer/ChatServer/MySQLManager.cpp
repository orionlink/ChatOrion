//
// Created by hwk on 2025/1/7.
//

#include "MySQLManager.h"

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
