//
// Created by hwk on 2025/1/14.
//

#include "LogicSystem.h"
#include "Settings.h"
#include "RedisManager.h"
#include "UserMgr.h"
#include "const.h"

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

    // 将登录该服务器的数量增加
    auto &setting = config::Settings::GetInstance();
    std::string self_server_name = setting.value("SelfServer/name").toString();

    std::string rd_res;
    bool ret = RedisManager::GetInstance()->hget(LOGIN_COUNT, self_server_name, rd_res);
    int count = 0;
    if (!rd_res.empty() && ret) {
        count = std::stoi(rd_res);
    }
    count++;
    auto count_str = std::to_string(count);
    RedisManager::GetInstance()->hset(LOGIN_COUNT, self_server_name, count_str);

    //session绑定用户uid
    session->set_user_id(uid);
    //为用户设置登录ip server的名字, 该用户和某个服务绑定
    std::string ipkey = USERIPPREFIX + uid_str;
    RedisManager::GetInstance()->set(ipkey, self_server_name);
    //uid和session绑定管理,方便以后踢人操作
    UserMgr::GetInstance()->SetUserSession(uid, session);

}