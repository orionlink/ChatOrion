//
// Created by hwk on 2025/1/9.
//

#include "StatusGrpcClient.h"

StatusConnectPool::StatusConnectPool(size_t pool_size, const std::string &host, const std::string &port)
    :_pool_size(pool_size), _host(host), _port(port), _is_stop(false)
{
    std::string url = _host + ":";
    url += _port;
    for (int i = 0; i < _pool_size; ++i)
    {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(url, grpc::InsecureChannelCredentials());

        _connections.push(StatusService::NewStub(channel));
    }
}

StatusConnectPool::~StatusConnectPool()
{
    std::lock_guard<std::mutex> lock(_mutex);
    close();
    while (!_connections.empty())
    {
        _connections.pop();
    }
}

std::unique_ptr<StatusService::Stub> StatusConnectPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condition_var.wait(lock, [this] {
        if (_is_stop) {
            return true;
        }
        return !_connections.empty();

    });
    //如果停止则直接返回空指针
    if (_is_stop) {
        return  nullptr;
    }

    auto context = std::move(_connections.front());
    _connections.pop();
    return context;
}

void StatusConnectPool::returnConnection(std::unique_ptr<StatusService::Stub> conn)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_is_stop) {
        return;
    }
    _connections.push(std::move(conn));
    _condition_var.notify_one();
}

void StatusConnectPool::close()
{
    _is_stop = true;
    _condition_var.notify_all();
}

StatusGrpcClient::StatusGrpcClient()
{
    auto settings = config::Settings::GetInstance();
    auto host = settings.value("StatusServer/host").toString();
    auto port = settings.value("StatusServer/port").toString();
    _pool.reset(new StatusConnectPool(5, host, port));
}

StatusGrpcClient::~StatusGrpcClient()
{
    _pool->close();
}

GetChatServerRes StatusGrpcClient::GetChatServer(int uid)
{
    GetChatServerReq request;
    GetChatServerRes response;
    request.set_uid(uid);
    auto stub = _pool->getConnection();
    Defer defer([this, &stub]
    {
        _pool->returnConnection(std::move(stub));
    });

    ClientContext context;
    grpc::Status status = stub->GetChatServer(&context, request, &response);
    if (status.ok())
    {
        return response;
    }
    else
    {
        response.set_error(ErrorCodes::RPCFailed);
        return response;
    }
}
