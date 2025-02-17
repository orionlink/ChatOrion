//
// Created by hwk on 2025/1/7.
//

#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H

#include "singleton.hpp"
#include "const.h"
#include "MySQLDao.h"

class MySQLManager : public Singleton<MySQLManager>
{
    friend class Singleton<MySQLManager>;
public:
    /**
     * 初始化数据库
     */
    bool init();

    int registerUser(const std::string& name, const std::string& email,  const std::string& pwd, const std::string& icon);
    bool checkUsernameEmailMatch(const std::string& name, const std::string & email);
    bool updatePassword(const std::string& name, const std::string& newpwd);

    /**
     * @return -1：SQL出错,　1: 用户不存在, 2: 密码错误, 0 校验成功
     */
    int checkUserLogin(const std::string& name, const std::string& pwd, UserInfo& userInfo);
    bool checkEmailLoginWithCode(const std::string& email, UserInfo& userInfo);
private:
    MySQLDao _dao;
};



#endif //MYSQLMANAGER_H
