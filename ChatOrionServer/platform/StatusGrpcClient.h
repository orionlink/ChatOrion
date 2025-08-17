//
// Created by hwk on 2025/1/9.
//

#ifndef STATUSGRPCCLIENT_H
#define STATUSGRPCCLIENT_H

#include <grpcpp/grpcpp.h>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <atomic>

#include "message.grpc.pb.h"
#include "const.h"
#include "Settings.h"
#include "singleton.hpp"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRes;
using message::RegisterChatServerReq;
using message::HeartbeatReq;
using message::MessageRes;
using message::StatusService;

class StatusConnectPool
{
public:
    StatusConnectPool(size_t pool_size, const std::string& host, const std::string& port);
    ~StatusConnectPool();

    std::unique_ptr<StatusService::Stub> getConnection();

    void returnConnection(std::unique_ptr<StatusService::Stub> conn);

    void close();
private:
    std::queue<std::unique_ptr<StatusService::Stub>> _connections;
    size_t _pool_size;
    std::string _host;
    std::string _port;
    std::atomic<bool> _is_stop;
    std::mutex _mutex;
    std::condition_variable _condition_var;
};

class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
    friend class Singleton<StatusGrpcClient>;
public:
    ~StatusGrpcClient();

    GetChatServerRes GetChatServer(int uid);

    MessageRes RegisterChatServer(const std::string& service_name, const std::string& host, const std::string& port);

    MessageRes Heartbeat(const std::string& service_name);
private:
    StatusGrpcClient();

    std::unique_ptr<StatusConnectPool> _pool;
};

#endif //STATUSGRPCCLIENT_H
