//
// Created by hwk on 2025/2/10.
//

#include "ChatGrpcClient.h"

#include <log.h>

#include "Settings.h"
#include "tools.h"
#include "RedisManager.h"
#include "MySQLManager.h"

ChatConPool::ChatConPool(size_t pool_size, std::string host, std::string port)
    : _pool_size(pool_size), _host(host), _port(port), _b_stop(false)
{
    for (size_t i = 0; i < _pool_size; ++i) {

        std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
            grpc::InsecureChannelCredentials());

        _connections.push(ChatService::NewStub(channel));
    }
}

std::unique_ptr<ChatService::Stub> ChatConPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] {
        if (_b_stop) {
            return true;
        }
        return !_connections.empty();
        });
    //如果停止则直接返回空指针
    if (_b_stop) {
        return  nullptr;
    }
    auto context = std::move(_connections.front());
    _connections.pop();
    return context;
}

void ChatConPool::returnConnection(std::unique_ptr<ChatService::Stub> context)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_b_stop) {
        return;
    }
    _connections.push(std::move(context));
    _cond.notify_one();
}

void ChatConPool::close()
{
    _b_stop = true;
    _cond.notify_all();
}

ChatGrpcClient::ChatGrpcClient()
{
    auto& settings = config::Settings::GetInstance();
    std::string servers = settings.value("ChatServers/name").toString();
    Tools::RemoveWhitespace(servers);

    std::stringstream ss(servers);
    std::vector<std::string> server_names;
    std::string server_name;
    while (std::getline(ss, server_name, ','))
    {
        server_names.push_back(server_name);
    }

    for (auto &server_name : server_names)
    {
        if (!settings.exists(server_name)) continue;

        std::string host = settings.value(server_name + "/" + "host").toString();
        std::string port = settings.value(server_name + "/" + "port").toString();
        std::string rpc_port = settings.value(server_name + "/" + "rpc_port").toString();
        std::string name = settings.value(server_name + "/" + "name").toString();

        std::string self_server_name = settings.value("SelfServer/name").toString();
        if (self_server_name == name) continue;

        _pools[name] = std::make_unique<ChatConPool>(5, host, rpc_port);
    }
}

AddFriendRsp ChatGrpcClient::NotifyAddFriend(std::string server_ip, const AddFriendReq &req)
{
    AddFriendRsp rsp;

    auto find_iter = _pools.find(server_ip);
    if (find_iter == _pools.end())
    {
        rsp.set_error(ErrorCodes::RPCFailed);
        LOG_ERROR << "在rpc请求池中找不到需要请求的服务 server_name: " << server_ip;
        return rsp;
    }

    auto &pool = find_iter->second;
    ClientContext context;
    auto stub = pool->getConnection();
    Status status = stub->NotifyAddFriend(&context, req, &rsp);
    Defer defercon([&stub, this, &pool]() {
        pool->returnConnection(std::move(stub));
    });

    if (!status.ok())
    {
        rsp.set_error(ErrorCodes::RPCFailed);
        LOG_ERROR << "rpc请求失败";
        return rsp;
    }

    return rsp;
}

AuthFriendRsp ChatGrpcClient::NotifyAuthFriend(std::string server_ip, const AuthFriendReq &req)
{
    AuthFriendRsp rsp;

    auto find_iter = _pools.find(server_ip);
    if (find_iter == _pools.end())
    {
        rsp.set_error(ErrorCodes::RPCFailed);
        LOG_ERROR << "在rpc请求池中找不到需要请求的服务 server_name: " << server_ip;
        return rsp;
    }

    auto &pool = find_iter->second;
    ClientContext context;
    auto stub = pool->getConnection();
    Status status = stub->NotifyAuthFriend(&context, req, &rsp);
    Defer defercon([&stub, this, &pool]() {
        pool->returnConnection(std::move(stub));
    });

    if (!status.ok())
    {
        rsp.set_error(ErrorCodes::RPCFailed);
        LOG_ERROR << "rpc请求失败";
        return rsp;
    }

    return rsp;
}

TextChatMsgRsp ChatGrpcClient::NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq &req)
{
    TextChatMsgRsp rsp;
    rsp.set_error(ErrorCodes::Success);

    Defer defer([&rsp, &req]()
    {
        rsp.set_fromuid(req.fromuid());
        rsp.set_touid(req.touid());
        rsp.set_msgid(req.msgid());
        rsp.set_content(req.content());
    });

    auto find_iter = _pools.find(server_ip);
    if (find_iter == _pools.end()) {
        LOG_ERROR << "在rpc请求池中找不到需要请求的服务 server_name: " << server_ip;
        return rsp;
    }

    auto& pool = find_iter->second;
    ClientContext context;
    auto stub = pool->getConnection();
    Status status = stub->NotifyTextChatMsg(&context, req, &rsp);
    Defer defercon([&stub, this, &pool]() {
        pool->returnConnection(std::move(stub));
    });

    if (!status.ok()) {
        LOG_ERROR << "rpc请求失败";
        rsp.set_error(ErrorCodes::RPCFailed);
        return rsp;
    }

    return rsp;
}

bool ChatGrpcClient::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo> &userinfo)
{
    //优先查redis中查询用户信息
    std::string info_str = "";
    bool b_base = RedisManager::GetInstance()->get(base_key, info_str);
    if (b_base) {
        Json::Reader reader;
        Json::Value root;
        reader.parse(info_str, root);
        userinfo->uid = root["uid"].asInt();
        userinfo->name = root["name"].asString();
        userinfo->pwd = root["pwd"].asString();
        userinfo->email = root["email"].asString();
        userinfo->nick = root["nick"].asString();
        userinfo->desc = root["desc"].asString();
        userinfo->sex = root["sex"].asInt();
        userinfo->icon = root["icon"].asString();
        std::cout << "ChatServiceImpl::GetBaseInfo  " << userinfo->uid << " name  is "
            << userinfo->name << " pwd is " << userinfo->pwd << " email is " << userinfo->email << std::endl;
    }
    else {
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
    }

    return true;
}
