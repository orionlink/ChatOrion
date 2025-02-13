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
    bool AddFriendApply(const int& from_uid, const int& to_uid);

    bool AuthFriendApply(const int& from_uid, const int& to_uid);

    bool AddFriend(const int& from_uid, const int& to_uid, std::string back_name);

    bool GetApplyList(int self_id, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int offset, int limit);

    bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo> >& user_info_list);
private:
    // 拆分 SQL 脚本为多个语句
    std::vector<std::string> splitSQLScript(const std::string& sql_content);

    std::unique_ptr<MySQLConnectPool> _pool;
};



#endif //MYSQLDAO_H
