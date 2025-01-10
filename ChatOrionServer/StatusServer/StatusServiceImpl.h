//
// Created by hwk on 2025/1/9.
//

#ifndef STATUSSERVICEIMPL_H
#define STATUSSERVICEIMPL_H

#include <unordered_map>
#include <mutex>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRes;
using message::StatusService;

class ChatServer
{
public:
    ChatServer():host(""), port(""), name(""), conn_count(0) {}
    ChatServer(const std::string& host, const std::string& port, const std::string& name):host(host), port(port), name(name), conn_count(0) {}

    ChatServer(const ChatServer& cs):host(cs.host), port(cs.port), name(cs.name), conn_count(cs.conn_count){}
    ChatServer& operator=(const ChatServer& cs)
    {
        if (&cs == this) {
            return *this;
        }

        host = cs.host;
        name = cs.name;
        port = cs.port;
        conn_count = cs.conn_count;
        return *this;
    }

    std::string host;
    std::string port;
    std::string name;
    int conn_count;
};

class StatusServiceImpl final : public StatusService::Service
{
public:
    StatusServiceImpl();

    Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
        GetChatServerRes* reply) override;
private:
    void insertToken(int uid, const std::string token);
    ChatServer getChatServer();

    std::unordered_map<std::string, ChatServer> _servers;
    std::mutex _servers_mutex;
};

#endif //STATUSSERVICEIMPL_H
