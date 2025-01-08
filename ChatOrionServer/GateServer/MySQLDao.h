//
// Created by hwk on 2025/1/7.
//

#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include "MySQLConnectPool.h"
#include "const.h"

class MySQLDao
{
public:
    MySQLDao();
    ~MySQLDao();
    int registerUser(const std::string& name, const std::string& email, const std::string& pwd);
    int registerTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon);
    bool checkEmail(const std::string& name, const std::string & email);
    bool updatePassword(const std::string& name, const std::string& newpwd);
    bool checkPassword(const std::string& name, const std::string& pwd, UserInfo& userInfo);
    bool testProcedure(const std::string& email, int& uid, std::string& name);
private:
    // 拆分 SQL 脚本为多个语句
    std::vector<std::string> splitSQLScript(const std::string& sql_content);

    std::unique_ptr<MySQLConnectPool> _pool;
};



#endif //MYSQLDAO_H
