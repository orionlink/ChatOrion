//
// Created by hwk on 2025/2/10.
//

#ifndef USERMGR_H
#define USERMGR_H

#include "singleton.hpp"

#include <unordered_map>
#include <memory>
#include <mutex>

class CSession;

class UserMgr : public Singleton<UserMgr>
{
    friend class Singleton<UserMgr>;
public:
    ~UserMgr();
    std::shared_ptr<CSession> GetSession(int uid);
    void SetUserSession(int uid, std::shared_ptr<CSession> session);
    void RemoveUserSession(int uid);
private:
    UserMgr();
    std::mutex _session_mtx;
    std::unordered_map<int, std::shared_ptr<CSession>> _uid_to_session;
};



#endif //USERMGR_H
