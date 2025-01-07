//
// Created by hwk on 2025/1/7.
//

#include "MySQLManager.h"

int MySQLManager::RegUser(const std::string& name, const std::string& email, const std::string& pwd,
    const std::string& icon)
{
}

bool MySQLManager::CheckEmail(const std::string& name, const std::string& email)
{
}

bool MySQLManager::UpdatePwd(const std::string& name, const std::string& email)
{
}

bool MySQLManager::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo)
{
}

bool MySQLManager::TestProcedure(const std::string& email, int& uid, std::string& name)
{
}
