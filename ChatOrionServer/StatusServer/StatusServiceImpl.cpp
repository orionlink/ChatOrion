//
// Created by hwk on 2025/1/9.
//

#include "StatusServiceImpl.h"
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
    auto settings = config::Settings::GetInstance();
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
}

Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request,
    GetChatServerRes* reply)
{
    std::string prefix("status server has received :  ");
    const auto &server = getChatServer();
    reply->set_host(server.host);
    reply->set_port(std::atoi(server.port.c_str()));
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());
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
