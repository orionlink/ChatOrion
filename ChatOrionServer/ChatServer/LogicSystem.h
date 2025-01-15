//
// Created by hwk on 2025/1/14.
//

#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include "singleton.hpp"
#include "CSession.h"
#include "const.h"
#include "BS_thread_pool.hpp"

class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    using msg_handle_callback = std::function<void(std::shared_ptr<CSession>, const std::string &msg_data)>;

    ~LogicSystem();

    void postMsgToQue(std::shared_ptr<LogicNode> msg);

    void registerMsgCallBacks(MSG_IDS msg_id, msg_handle_callback handler);
    void unregisterMsgCallBacks(MSG_IDS msg_id, msg_handle_callback handler);
private:
    LogicSystem();

    void LoginHandler(std::shared_ptr<CSession> session, const std::string &msg_data);
private:
    BS::thread_pool<BS::tp::pause> _pool;  // 线程池
    bool _b_stop;
    std::map<MSG_IDS, msg_handle_callback> _handle_callbacks;
};



#endif //LOGICSYSTEM_H
