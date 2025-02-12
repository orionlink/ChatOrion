#include <iostream>
#include <csignal>
#include <mutex>
#include <thread>
#include <tools.h>

#include "AsioIOServicePool.h"
#include "Settings.h"
#include "const.h"
#include "CServer.h"
#include "RedisManager.h"
#include "log.h"
#include "StatusGrpcClient.h"
#include "ChatServiceImpl.h"

static void use(char* argv[])
{
    std::cout << argv[0] << " arg1: 服务器标识" << std::endl;
    std::cout << "示例: " <<  argv[0] << " 1"<< std::endl;
}

// ./ChatServer 1 8080, 1 为服务器id标识, 拼接字符串以后就是 ChatServer1
int main(int argc, char* argv[])
{
    try
    {
        auto &setting = config::Settings::GetInstance();
        setting.setFileName("config.ini");
        setting.load();

        auto logger = Log::GetInstance();
        if (!logger->Initialize(argv[0])) {
            std::cerr << "Failed to initialize logging" << std::endl;
            return -1;
        }

        unsigned short port;

        std::string server_name = "ChatServer";
        std::string server_name_id = "";
        if (argc == 2)
        {
            server_name_id = argv[1];
        }
        else
        {
            use(argv);
            return -2;
        }
        server_name += server_name_id;
        LOG_INFO << "server_name: " << server_name;

        port = setting.value(server_name + "/port").toInt();

        // 保存自己的服务信息到内存中
        setting.setValue("SelfServer/name", server_name);
        setting.setValue("SelfServer/host", "0.0.0.0");
        setting.setValue("SelfServer/port", std::to_string(port));

        // 向状态服务器注册自己
        std::vector<std::string> localIPs = Tools::GetLocalIPs();
        std::string localIP = "";
        if (!localIPs.empty())
        {
            localIP = localIPs[0];
        }
        MessageRes message_res = StatusGrpcClient::GetInstance()->RegisterChatServer(server_name, localIP, std::to_string(port));
        if (message_res.error() != message::Success)
        {
            LOG_ERROR << "服务注册失败: error: " <<  message_res.error();
            return -3;
        }

        // 将登录数设置为0
        RedisManager::GetInstance()->hset(LOGIN_COUNT, server_name, "0");

        // 开始启动rpc服务
        ChatServiceImpl service;
        grpc::ServerBuilder builder;
        // 监听端口和添加服务
        std::string server_address = "0.0.0.0:" + std::to_string(setting.value(server_name + "/rpc_port").toInt());
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        // 构建并启动gRPC服务器
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        LOG_INFO << "RPC Server listening on " << server_address;

        //单独启动一个线程处理grpc服务
        std::thread  grpc_server_thread([&server]() {
            server->Wait();
        });

        auto pool = AsioIOServicePool::GetInstance();
        boost::asio::io_context context;
        boost::asio::signal_set signals(context, SIGINT, SIGTERM);
        signals.async_wait([&context, &pool, &server](auto, auto)
        {
            context.stop();
            pool->stop();
            server->Shutdown();
        });

        CServer s(context, server_name, port);
        context.run();

        RedisManager::GetInstance()->hdel(LOGIN_COUNT, server_name);
        RedisManager::GetInstance()->close();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
