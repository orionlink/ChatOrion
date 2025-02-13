//
// Created by hwk on 2025/1/7.
//

#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include "MySQLConnectPool.h"
#include "const.h"

struct UserInfo
{
    UserInfo():name(""), pwd(""),uid(0),email(""){}
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};

class MySQLDao
{
public:
    MySQLDao();
    ~MySQLDao();
    bool init();
    int registerUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool checkUsernameEmailMatch(const std::string& name, const std::string & email);
    bool updatePassword(const std::string& name, const std::string& newpwd);
    int checkUserLogin(const std::string& name, const std::string& pwd, UserInfo& userInfo);
    bool checkEmailLoginWithCode(const std::string& email, UserInfo& userInfo);
private:
    // 拆分 SQL 脚本为多个语句
    std::vector<std::string> splitSQLScript(const std::string& sql_content);

    std::unique_ptr<MySQLConnectPool> _pool;
};



#endif //MYSQLDAO_H
