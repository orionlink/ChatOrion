//
// Created by hwk on 2025/1/9.
//

#ifndef STATUSSERVICEIMPL_H
#define STATUSSERVICEIMPL_H

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRes;
using message::StatusService;

struct ChatServer
{
    std::string host;
    std::string port;
};

class StatusServiceImpl final : public StatusService::Service
{
public:
    StatusServiceImpl();

    Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
        GetChatServerRes* reply) override;
private:
    std::vector<ChatServer> _servers;
    int _server_index;
};

#endif //STATUSSERVICEIMPL_H
