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
    std::shared_ptr<UserInfo> GetUser(int uid);
    std::shared_ptr<UserInfo> GetUser(std::string name);

    bool AddFriendApply(const int& from_uid, const int& to_uid);

    bool AuthFriendApply(const int& from_uid, const int& to_uid);

    bool AddFriend(const int& from_uid, const int& to_uid, std::string back_name);
private:
    MySQLDao _dao;
};



#endif //MYSQLMANAGER_H
