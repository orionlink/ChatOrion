//
// Created by hwk on 2025/1/7.
//

#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H

#include "singleton.hpp"
#include "const.h"
#include "MySQLDao.h"
#include "singleton.hpp"

struct UserInfo {
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};

class MySQLManager : public Singleton<MySQLManager>
{
    friend class Singleton<MySQLManager>;
public:
    int RegUser(const std::string& name, const std::string& email,  const std::string& pwd, const std::string& icon);
    bool CheckEmail(const std::string& name, const std::string & email);
    bool UpdatePwd(const std::string& name, const std::string& email);
    bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
    bool TestProcedure(const std::string &email, int& uid, std::string & name);
private:
    MySQLDao  _dao;
};



#endif //MYSQLMANAGER_H
