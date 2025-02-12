//
// Created by hwk on 2025/1/9.
//

#ifndef STATUSSERVICEIMPL_H
#define STATUSSERVICEIMPL_H

#include <unordered_map>
#include <mutex>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRes;
using message::RegisterChatServerReq;
using message::HeartbeatReq;
using message::MessageRes;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;

class ChatServer
{
public:
    ChatServer():host(""), port(""), name(""), conn_count(0), last_heartbeat(0) {}
    ChatServer(const std::string& host, const std::string& port, const std::string& name):host(host), port(port), name(name), conn_count(0) {}

    ChatServer(const ChatServer& cs):host(cs.host), port(cs.port), name(cs.name), conn_count(cs.conn_count), last_heartbeat(cs.last_heartbeat){}
    ChatServer& operator=(const ChatServer& cs)
    {
        if (&cs == this) {
            return *this;
        }

        host = cs.host;
        name = cs.name;
        port = cs.port;
        conn_count = cs.conn_count;
        last_heartbeat = cs.last_heartbeat;
        return *this;
    }

    std::string host;
    std::string port;
    std::string name;
    int conn_count;
    long long last_heartbeat;
};

class StatusServiceImpl final : public StatusService::Service
{
public:
    StatusServiceImpl();
    ~StatusServiceImpl();

    Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
        GetChatServerRes* reply) override;

    Status Login(ServerContext* context, const LoginReq* request,
    LoginRsp* reply) override;

    Status RegisterChatServer(ServerContext* context, const RegisterChatServerReq* request, MessageRes* reply) override;

    Status Heartbeat(ServerContext* context, const HeartbeatReq* request, MessageRes* reply) override;
private:
    void insertToken(int uid, const std::string token);
    ChatServer getChatServer();
    void checkHeartbeat();

    static int64_t getCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    std::unordered_map<std::string, ChatServer> _servers;
    std::mutex _servers_mutex;

    std::thread _heartbeat_checker;  // 心跳检测线程
    std::atomic<bool> _running{true}; // 控制线程启停
};

#endif //STATUSSERVICEIMPL_H
