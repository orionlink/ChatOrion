//
// Created by hwk on 2025/1/2.
//

#ifndef VERIFY_GRPC_CLIENT_H
#define VERIFY_GRPC_CLIENT_H

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

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class GPCConnectPool
{
public:
    GPCConnectPool(int pool_size, const std::string& host, const std::string& port);

    ~GPCConnectPool();

    std::unique_ptr<VarifyService::Stub> getConnection();
    void returnConnection(std::unique_ptr<VarifyService::Stub> stub);

    void close();
private:
    std::queue<std::unique_ptr<VarifyService::Stub>> _stub_queue;
    int _pool_size;
    std::mutex _mtx;
    std::condition_variable _condition_var;
    std::atomic<bool> _b_stop;
    std::string _host;
    std::string _port;
};

class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:
    GetVarifyRsp GetVarifyCode(const std::string &email);

private:
    VerifyGrpcClient();

    std::unique_ptr<GPCConnectPool> _rcp_pool;
};

#endif //VERIFY_GRPC_CLIENT_H
