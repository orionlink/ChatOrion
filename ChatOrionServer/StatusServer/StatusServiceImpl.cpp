//
// Created by hwk on 2025/1/9.
//

#include "StatusServiceImpl.h"

#include "log.h"
#include "Settings.h"
#include "const.h"
#include "tools.h"
#include "RedisManager.h"

#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string generate_unique_string()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();

    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);

    return unique_string;
}

StatusServiceImpl::StatusServiceImpl()
{
#if 0
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

        ChatServer chat_server;
        chat_server.host = settings.value(server_name + "/" + "host").toString();
        chat_server.port = settings.value(server_name + "/" + "port").toString();
        chat_server.name = settings.value(server_name + "/" + "name").toString();
        _servers[server_name] = chat_server;
    }
#endif

    // 启动心跳检测线程
    _heartbeat_checker = std::thread([this] {
        while (_running) {
            std::this_thread::sleep_for(std::chrono::seconds(10)); // 每10秒检测一次
            checkHeartbeat();
        }
    });
}

StatusServiceImpl::~StatusServiceImpl()
{
    _running = false;
    if (_heartbeat_checker.joinable()) {
        _heartbeat_checker.join();
    }
}

Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request,
                                        GetChatServerRes* reply)
{
    std::string prefix("status server has received :  ");

    try
    {
        const auto &server = getChatServer();
        reply->set_host(server.host);
        reply->set_port(std::atoi(server.port.c_str()));
        reply->set_error(ErrorCodes::Success);
        reply->set_token(generate_unique_string());

        insertToken(request->uid(), reply->token());
    }
    catch (std::exception& ex)
    {
        LOG_ERROR << ex.what();
    }

    return Status::OK;
}

Status StatusServiceImpl::Login(ServerContext *context, const LoginReq *request, LoginRsp *reply)
{
    auto uid = request->uid();
    auto token = request->token();

    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    std::string token_value = "";
    bool success = RedisManager::GetInstance()->get(token_key, token_value);
    if (success) {
        reply->set_error(ErrorCodes::UidInvalid);
        return Status::OK;
    }

    if (token_value != token) {
        reply->set_error(ErrorCodes::TokenInvalid);
        return Status::OK;
    }
    reply->set_error(ErrorCodes::Success);
    reply->set_uid(uid);
    reply->set_token(token);
    return Status::OK;
}

Status StatusServiceImpl::RegisterChatServer(ServerContext *context, const RegisterChatServerReq *request,
    MessageRes *reply)
{
    std::lock_guard<std::mutex> lock(_servers_mutex);

    // 检查参数是否合法
    if (request->port().empty() || request->name().empty()) {
        reply->set_error(message::InvalidArgument);
        return Status::OK;
    }

    std::string peer_host = request->host();
    if (peer_host.empty())
    {
        peer_host = context->peer();
    }

    // 添加或更新服务器信息
    ChatServer server;
    server.host = peer_host;
    server.port = request->port();
    server.name = request->name();
    server.last_heartbeat = getCurrentTimestamp();

    _servers[server.name] = server;

    reply->set_error(message::Success);

    LOG_INFO << "服务：" << server.name << " 注册成功，初始心跳时间：" << server.last_heartbeat << " ip-port: " << server.host << ":" << server.port;
    return Status::OK;
}

void StatusServiceImpl::insertToken(int uid, const std::string token)
{
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPREFIX + uid_str;
    RedisManager::GetInstance()->set(token_key, token);
}

ChatServer StatusServiceImpl::getChatServer()
{
    std::lock_guard<std::mutex> lock(_servers_mutex);

    if (_servers.empty())
    {
        throw std::runtime_error("No available chat servers");
    }
    // 获取 Redis 连接数的辅助函数
    auto getConnectionCount = [](const std::string& server_name)
    {
        std::string connection_count;
        RedisManager::GetInstance()->hget(LOGIN_COUNT, server_name, connection_count);
        return connection_count.empty() ? INT_MAX : std::stoi(connection_count);
    };

    auto minServerIt = _servers.begin();
    minServerIt->second.conn_count = getConnectionCount(minServerIt->second.name);
    auto minServer = &minServerIt->second;
    for (auto it = _servers.begin(); it != _servers.end(); it++)
    {
        it->second.conn_count = getConnectionCount(it->second.name);

        if (it->second.conn_count < minServer->conn_count)
        {
            minServer = &it->second;
        }
    }

    return *minServer;
}

void StatusServiceImpl::checkHeartbeat()
{
    std::lock_guard<std::mutex> lock(_servers_mutex);
    auto now = getCurrentTimestamp();

    for (auto it = _servers.begin(); it != _servers.end();)
    {
        int64_t time_diff = now - it->second.last_heartbeat;

        LOG_INFO << "服务：" << it->first
                << " 最后心跳时间：" << it->second.last_heartbeat
                << " 当前时间：" << now
                << " 时间差：" << time_diff << "秒";

        if (time_diff > 30)  // 30秒超时
        {
            LOG_WARNING << "服务：" << it->first << " 已经离线，时间差：" << time_diff << "秒";
            it = _servers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

Status StatusServiceImpl::Heartbeat(ServerContext *context, const HeartbeatReq *request, MessageRes *reply)
{
    std::lock_guard<std::mutex> lock(_servers_mutex);

    auto it = _servers.find(request->name());
    if (it == _servers.end()) {
        reply->set_error(message::ServerNotFound);
        return Status::OK;
    }

    auto now = getCurrentTimestamp();
    LOG_INFO << "服务：" << request->name()
             << " 更新心跳, 旧时间：" << it->second.last_heartbeat
             << " 新时间：" << now;

    it->second.last_heartbeat = now;
    reply->set_error(message::Success);
    return Status::OK;
}
