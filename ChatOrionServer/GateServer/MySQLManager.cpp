//
// Created by hwk on 2025/1/7.
//

#include "MySQLManager.h"

int MySQLManager::registerUser(const std::string& name, const std::string& email,  const std::string& pwd, const std::string& icon)
{
    return _dao.registerUser(name, email, pwd);
}

bool MySQLManager::checkUsernameEmailMatch(const std::string& name, const std::string& email)
{
    return _dao.checkUsernameEmailMatch(name, email);
}

bool MySQLManager::updatePassword(const std::string& name, const std::string& newpwd)
{
    return _dao.updatePassword(name, newpwd);
}

int MySQLManager::checkUserLogin(const std::string& name, const std::string& pwd, UserInfo& userInfo)
{
    return _dao.checkUserLogin(name, pwd, userInfo);
}

bool MySQLManager::checkEmailLoginWithCode(const std::string &email, UserInfo &userInfo)
{
    return _dao.checkEmailLoginWithCode(email, userInfo);
}
