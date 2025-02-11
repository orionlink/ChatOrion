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
