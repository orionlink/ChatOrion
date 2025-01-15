//
// Created by hwk on 2025/1/14.
//

#include "LogicSystem.h"

#include <RedisManager.h>

LogicSystem::LogicSystem()
    :_b_stop(false)
{
    registerMsgCallBacks(MSG_IDS::MSG_CHAT_LOGIN, std::bind(&LogicSystem::LoginHandler, this,
        std::placeholders::_1, std::placeholders::_2));
}

LogicSystem::~LogicSystem()
{
    _b_stop = true;
    _pool.wait();  // 等待所有任务完成
}

void LogicSystem::registerMsgCallBacks(MSG_IDS msg_id, msg_handle_callback handler)
{
    _handle_callbacks[msg_id] = handler;
}

void LogicSystem::unregisterMsgCallBacks(MSG_IDS msg_id, msg_handle_callback handler)
{
    _handle_callbacks.erase(msg_id);
}

void LogicSystem::postMsgToQue(std::shared_ptr<LogicNode> msg)
{
    if (_b_stop) {
        return;  // 如果系统已停止，不再处理新任务
    }

    // 获取任务 ID
    short msg_id = msg->_recv_node->_msg_id;

    // 查找任务回调函数
    auto call_back_iter = _handle_callbacks.find(static_cast<MSG_IDS>(msg_id));
    if (call_back_iter == _handle_callbacks.end())
    {
        std::cout << "msg id [" << msg_id << "] handler not found" << std::endl;
        return;
    }

    // 提交任务到线程池
    _pool.detach_task([this, msg, call_back_iter]() {
        call_back_iter->second(msg->_session, msg->_recv_node->_data);
    });
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, const std::string &msg_data)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid = root["uid"].asInt();
    auto token = root["token"].asString();
    std::cout << "user login uid is  " << uid << " user token  is "
        << token << std::endl;

    Json::Value  rtvalue;
    Defer defer([this, &rtvalue, session]() {
        std::string return_str = rtvalue.toStyledString();
        session->send(return_str, MSG_CHAT_LOGIN_RSP);
        });

    //从redis获取用户token是否正确
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool success = RedisManager::GetInstance()->get(token_key, token_value);
    if (!success) {
        rtvalue["error"] = ErrorCodes::UidInvalid;
        return ;
    }

    if (token_value != token) {
        rtvalue["error"] = ErrorCodes::TokenInvalid;
        return ;
    }

    rtvalue["error"] = ErrorCodes::Success;
}