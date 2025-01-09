//
// Created by hwk on 2025/1/9.
//

#include "StatusServiceImpl.h"
#include "Settings.h"
#include "const.h"

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

StatusServiceImpl::StatusServiceImpl() : _server_index(0)
{
    auto settings = config::Settings::GetInstance();
    ChatServer server;
    server.host = settings.value("ChatServer1/host").toString();
    server.port = settings.value("ChatServer1/port").toString();
    _servers.push_back(server);

    server.host = settings.value("ChatServer2/host").toString();
    server.port = settings.value("ChatServer2/port").toString();
    _servers.push_back(server);
}

Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request,
    GetChatServerRes* reply)
{
    std::string prefix("status server has received :  ");
    _server_index = (_server_index++) % (_servers.size());
    auto &server = _servers[_server_index];
    reply->set_host(server.host);
    reply->set_port(std::atoi(server.port.c_str()));
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());
    return Status::OK;
}
