//
// Created by hwk on 2025/1/14.
//

#include "LogicSystem.h"
#include "Settings.h"
#include "RedisManager.h"
#include "UserMgr.h"
#include "const.h"
#include "MySQLManager.h"

#include <RedisManager.h>

LogicSystem::LogicSystem()
    :_b_stop(false)
{
    registerMsgCallBacks(MSG_IDS::MSG_CHAT_LOGIN, std::bind(&LogicSystem::LoginHandler, this,
        std::placeholders::_1, std::placeholders::_2));

    registerMsgCallBacks(MSG_IDS::ID_SEARCH_USER_REQ, std::bind(&LogicSystem::SearchInfoHandler, this,
        std::placeholders::_1, std::placeholders::_2));

    registerMsgCallBacks(MSG_IDS::ID_ADD_FRIEND_REQ, std::bind(&LogicSystem::AddFriendApplyHandler, this,
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

    Defer defer([this, &rtvalue, session]()
    {
        std::string return_str = rtvalue.toStyledString();
        session->send(return_str, MSG_CHAT_LOGIN_RSP);
    });

    //从redis获取用户token是否正确
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool success = RedisManager::GetInstance()->get(token_key, token_value);
    if (!success) {
        rtvalue["error_msg"] = "uid无效";
        rtvalue["error"] = ErrorCodes::UidInvalid;
        return ;
    }

    if (token_value != token) {
        rtvalue["error_msg"] = "Token失效";
        rtvalue["error"] = ErrorCodes::TokenInvalid;
        return ;
    }

    rtvalue["error"] = ErrorCodes::Success;

    // 获取用户信息
    auto user_info = std::make_shared<UserInfo>();
    bool b_base = GetBaseInfo(uid, user_info);
    if (!b_base)
    {
        rtvalue["error_msg"] = "uid无效";
        rtvalue["error"] = ErrorCodes::UidInvalid;
        return;
    }
    rtvalue["uid"] = uid;
    rtvalue["pwd"] = user_info->pwd;
    rtvalue["name"] = user_info->name;
    rtvalue["email"] = user_info->email;
    rtvalue["nick"] = user_info->nick;
    rtvalue["desc"] = user_info->desc;
    rtvalue["sex"] = user_info->sex;
    rtvalue["icon"] = user_info->icon;


    // 从数据库获取申请列表

    // 获取好友列表

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

    return;
}

void LogicSystem::SearchInfoHandler(std::shared_ptr<CSession> session, const std::string &msg_data)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(msg_data, root);
    auto uid_str = root["uid"].asString();
    std::cout << "user SearchInfo uid is  " << uid_str << std::endl;

    Json::Value rtvalue;

    Defer defer([this, &rtvalue, &session]()
    {
        std::string return_str = rtvalue.toStyledString();
        session->send(return_str, ID_SEARCH_USER_RSP);
    });

    GetUserByName(uid_str, rtvalue);

    // bool is_digit = isPureDigit(uid_str);
    //
    // if (is_digit)
    // {
    //     GetUserByUid(uid_str, rtvalue);
    // }
    // else
    // {
    //     GetUserByName(uid_str, rtvalue);
    // }
}

void LogicSystem::AddFriendApplyHandler(std::shared_ptr<CSession> session, const std::string &msg_data)
{

}

bool LogicSystem::isPureDigit(const std::string &str)
{
    for (char c : str)
    {
        if (!std::isdigit(c))
        {
            return false;
        }
    }
    return true;
}

bool LogicSystem::GetBaseInfo(int uid, std::shared_ptr<UserInfo> &userinfo)
{
    //优先查redis中查询用户信息
    std::string info_str = "";
    std::string base_key = USER_BASE_INFO + std::to_string(uid);
    bool b_base = RedisManager::GetInstance()->get(base_key, info_str);
    if (b_base)
    {
        Json::Value root;
        Json::Reader reader;
        reader.parse(info_str, root);
        userinfo->uid = root["uid"].asInt();
        userinfo->name = root["name"].asString();
        userinfo->pwd = root["pwd"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
        std::cout << "user login uid is  " << userinfo->uid << " name  is "
            << userinfo->name << " pwd is " << userinfo->pwd << " email is " << userinfo->email << std::endl;

        return true;
    }
    else
    {
        //redis中没有则查询mysql
        //查询数据库
        std::shared_ptr<UserInfo> user_info = nullptr;
        user_info = MySQLManager::GetInstance()->GetUser(uid);
        if (user_info == nullptr) {
            return false;
        }

        userinfo = user_info;

        //将数据库内容写入redis缓存
        Json::Value redis_root;
        redis_root["uid"] = uid;
        redis_root["pwd"] = userinfo->pwd;
        redis_root["name"] = userinfo->name;
        redis_root["email"] = userinfo->email;
        redis_root["nick"] = userinfo->nick;
        redis_root["desc"] = userinfo->desc;
        redis_root["sex"] = userinfo->sex;
        redis_root["icon"] = userinfo->icon;
        RedisManager::GetInstance()->set(base_key, redis_root.toStyledString());

        return true;
    }
}

void LogicSystem::GetUserByUid(const std::string &uid_str, Json::Value &rt_value)
{
    rt_value["error"] = ErrorCodes::Success;

    std::string base_key = USER_BASE_INFO + uid_str;

    //优先查redis中查询用户信息
    std::string info_str = "";
    bool b_base = RedisManager::GetInstance()->get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        auto uid = root["uid"].asInt();
        auto name = root["name"].asString();
        auto pwd = root["pwd"].asString();
        auto email = root["email"].asString();
        auto nick = root["nick"].asString();
        auto desc = root["desc"].asString();
        auto sex = root["sex"].asInt();
        auto icon = root["icon"].asString();
        std::cout << "user  uid is  " << uid << " name  is "
            << name << " pwd is " << pwd << " email is " << email <<" icon is " << icon << std::endl;

        rt_value["uid"] = uid;
        rt_value["pwd"] = pwd;
        rt_value["name"] = name;
        rt_value["email"] = email;
        rt_value["nick"] = nick;
        rt_value["desc"] = desc;
        rt_value["sex"] = sex;
        rt_value["icon"] = icon;
        return;
    }

    auto uid = std::stoi(uid_str);
    //redis中没有则查询mysql
    //查询数据库
    std::shared_ptr<UserInfo> user_info = nullptr;
    user_info = MySQLManager::GetInstance()->GetUser(uid);
    if (user_info == nullptr) {
        rt_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    //将数据库内容写入redis缓存
    Json::Value redis_root;
    redis_root["uid"] = user_info->uid;
    redis_root["pwd"] = user_info->pwd;
    redis_root["name"] = user_info->name;
    redis_root["email"] = user_info->email;
    redis_root["nick"] = user_info->nick;
    redis_root["desc"] = user_info->desc;
    redis_root["sex"] = user_info->sex;
    redis_root["icon"] = user_info->icon;

    RedisManager::GetInstance()->set(base_key, redis_root.toStyledString());

    //返回数据
    rt_value["uid"] = user_info->uid;
    rt_value["pwd"] = user_info->pwd;
    rt_value["name"] = user_info->name;
    rt_value["email"] = user_info->email;
    rt_value["nick"] = user_info->nick;
    rt_value["desc"] = user_info->desc;
    rt_value["sex"] = user_info->sex;
    rt_value["icon"] = user_info->icon;
}

void LogicSystem::GetUserByName(const std::string &username_str, Json::Value &rt_value)
{
    rt_value["error"] = ErrorCodes::Success;

    std::string base_key = USER_BASE_INFO + username_str;

    //优先查redis中查询用户信息
    std::string info_str = "";
    bool b_base = RedisManager::GetInstance()->get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        auto uid = root["uid"].asInt();
        auto name = root["name"].asString();
        auto pwd = root["pwd"].asString();
        auto email = root["email"].asString();
        auto nick = root["nick"].asString();
        auto desc = root["desc"].asString();
        auto sex = root["sex"].asInt();
        auto icon = root["icon"].asString();
        std::cout << "user  uid is  " << uid << " name  is "
            << name << " pwd is " << pwd << " email is " << email <<" icon is " << icon << std::endl;

        rt_value["uid"] = uid;
        rt_value["pwd"] = pwd;
        rt_value["name"] = name;
        rt_value["email"] = email;
        rt_value["nick"] = nick;
        rt_value["desc"] = desc;
        rt_value["sex"] = sex;
        rt_value["icon"] = icon;
        return;
    }

    //redis中没有则查询mysql
    //查询数据库
    std::shared_ptr<UserInfo> user_info = nullptr;
    user_info = MySQLManager::GetInstance()->GetUser(username_str);
    if (user_info == nullptr) {
        rt_value["error"] = ErrorCodes::UidInvalid;
        return;
    }

    //将数据库内容写入redis缓存
    Json::Value redis_root;
    redis_root["uid"] = user_info->uid;
    redis_root["pwd"] = user_info->pwd;
    redis_root["name"] = user_info->name;
    redis_root["email"] = user_info->email;
    redis_root["nick"] = user_info->nick;
    redis_root["desc"] = user_info->desc;
    redis_root["sex"] = user_info->sex;
    redis_root["icon"] = user_info->icon;

    RedisManager::GetInstance()->set(base_key, redis_root.toStyledString());

    //返回数据
    rt_value["uid"] = user_info->uid;
    rt_value["pwd"] = user_info->pwd;
    rt_value["name"] = user_info->name;
    rt_value["email"] = user_info->email;
    rt_value["nick"] = user_info->nick;
    rt_value["desc"] = user_info->desc;
    rt_value["sex"] = user_info->sex;
    rt_value["icon"] = user_info->icon;
}
