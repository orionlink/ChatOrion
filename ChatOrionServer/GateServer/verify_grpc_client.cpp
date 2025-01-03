//
// Created by hwk on 2025/1/4.
//

#include "verify_grpc_client.h"

GPCConnectPool::GPCConnectPool(int pool_size, const std::string& host, const std::string& port)
    :_pool_size(pool_size), _b_stop(false), _host(host), _port(port)
{
    std::string url = host;
    url += ":" + port;

    for (int i = 0; i < _pool_size; i++)
    {
        std::shared_ptr<grpc::Channel> channal = grpc::CreateChannel(url, grpc::InsecureChannelCredentials());
        _stub_queue.push(std::unique_ptr<VarifyService::Stub>(VarifyService::NewStub(channal)));
    }
}

GPCConnectPool::~GPCConnectPool()
{
    std::lock_guard<std::mutex> lock(_mtx);
    close();
    while (!_stub_queue.empty()) {
        _stub_queue.pop();
    }
}

std::unique_ptr<VarifyService::Stub> GPCConnectPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _condition_var.wait(lock, [this]
    {
        if (_b_stop)
            return true;
        return !_stub_queue.empty();
    });

    //如果停止则直接返回空指针
    if (_b_stop) {
        return  nullptr;
    }

    auto stub = std::move(_stub_queue.front());
    _stub_queue.pop();
    return stub;
}

void GPCConnectPool::returnConnection(std::unique_ptr<VarifyService::Stub> stub)
{
    std::unique_lock<std::mutex> lock(_mtx);
    if (_b_stop) {
        return;
    }
    _stub_queue.push(std::move(stub));
    _condition_var.notify_one();
}

void GPCConnectPool::close()
{
    _b_stop = true;
    _condition_var.notify_all();
}

GetVarifyRsp VerifyGrpcClient::GetVarifyCode(const std::string& email)
{
    grpc::ClientContext context;
    GetVarifyReq request;
    GetVarifyRsp response;
    request.set_email(email);
    auto stub = _rcp_pool->getConnection();
    Status status = stub->GetVarifyCode(&context, request, &response);
    if (status.ok())
    {
        return response;
    }
    else {
        response.set_error(ErrorCodes::RPCFailed);
        return response;
    }
}

VerifyGrpcClient::VerifyGrpcClient()
{
    config::Settings& settings = config::Settings::GetInstance();
    unsigned int port = settings.value("VarifyServer/Port", 50051).toInt();
    std::string host = "127.0.0.1";
    std::string port_str = std::to_string(port);

    _rcp_pool = std::unique_ptr<GPCConnectPool>(new GPCConnectPool(5, host, port_str));
}
