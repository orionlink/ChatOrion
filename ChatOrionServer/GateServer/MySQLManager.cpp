//
// Created by hwk on 2025/1/7.
//

#include "MySQLManager.h"

int MySQLManager::registerUser(const std::string& name, const std::string& email,  const std::string& pwd, const std::string& icon)
{
    return _dao.registerUser(name, email, pwd);
}

bool MySQLManager::checkEmail(const std::string& name, const std::string& email)
{
}

bool MySQLManager::updatePassword(const std::string& name, const std::string& email)
{
}

bool MySQLManager::checkPassword(const std::string& email, const std::string& pwd, UserInfo& userInfo)
{
}

bool MySQLManager::testProcedure(const std::string& email, int& uid, std::string& name)
{
}
