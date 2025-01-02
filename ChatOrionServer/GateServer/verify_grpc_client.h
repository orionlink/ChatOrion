//
// Created by hwk on 2025/1/2.
//

#ifndef VERIFY_GRPC_CLIENT_H
#define VERIFY_GRPC_CLIENT_H

#include <grpcpp/grpcpp.h>
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

class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{
    friend class Singleton<VerifyGrpcClient>;
public:
    GetVarifyRsp GetVarifyCode(const std::string &email)
    {
        grpc::ClientContext context;
        GetVarifyReq request;
        GetVarifyRsp response;
        request.set_email(email);
        Status status = _stub->GetVarifyCode(&context, request, &response);
        if (status.ok())
        {
            return response;
        }
        else {
            response.set_error(ErrorCodes::RPCFailed);
            return response;
        }
    }

    void setUrl(const std::string &url)
    {
        _url = url;
    }
private:
    VerifyGrpcClient()
    {
        config::Settings settings("config.ini");
        unsigned int port = settings.value("VarifyServer/Port", 50051).toInt();
        _url = "127.0.0.1";
        _url += ":" + std::to_string(port);

        std::shared_ptr<grpc::Channel> channal = grpc::CreateChannel(_url, grpc::InsecureChannelCredentials());
        _stub = message::VarifyService::NewStub(channal);
    }

    std::unique_ptr<message::VarifyService::Stub> _stub;
    std::string _url{"127.0.0.1:50051"};
};

#endif //VERIFY_GRPC_CLIENT_H
