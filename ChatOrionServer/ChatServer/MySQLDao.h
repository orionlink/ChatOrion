//
// Created by hwk on 2025/1/7.
//

#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include "MySQLConnectPool.h"
#include "const.h"
#include "data.h"

class MySQLDao
{
public:
    MySQLDao();
    ~MySQLDao();

    std::shared_ptr<UserInfo> GetUser(int uid);
    std::shared_ptr<UserInfo> GetUser(std::string name);
private:
    // 拆分 SQL 脚本为多个语句
    std::vector<std::string> splitSQLScript(const std::string& sql_content);

    std::unique_ptr<MySQLConnectPool> _pool;
};



#endif //MYSQLDAO_H
