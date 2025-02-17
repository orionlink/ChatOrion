#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <memory>
#include <string>
#include <thread>
#include <boost/asio.hpp>

#include "log.h"
#include "const.h"
#include "Settings.h"
#include "hiredis/hiredis.h"
#include "RedisManager.h"
#include "AsioIOServicePool.h"
#include "StatusServiceImpl.h"

void RunServer(const std::string& host, const std::string& port)
{
    std::string server_address(host+":"+ port);
    StatusServiceImpl service;

    grpc::ServerBuilder builder;
    // 监听端口和添加服务
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    // 构建并启动gRPC服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    LOG_INFO << "Server listening on " << server_address;

    // 创建Boost.Asio的io_context
    boost::asio::io_context io_context;
    // 创建signal_set用于捕获SIGINT
    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);

    // 设置异步等待SIGINT信号
    signals.async_wait([&server](const boost::system::error_code& error, int signal_number) {
        if (!error) {
            LOG_INFO << "Shutting down server...";
            server->Shutdown(); // 优雅地关闭服务器
        }
        });

    // 在单独的线程中运行io_context
    std::thread([&io_context]() { io_context.run(); }).detach();

    // 等待服务器关闭
    server->Wait();
    io_context.stop(); // 停止io_context
}

int main(int argc, char* argv[])
{
    auto& settings = config::Settings::GetInstance();
    settings.setFileName("config.ini");
    settings.load();

    auto logger = Log::GetInstance();
    if (!logger->Initialize(argv[0])) {
        std::cerr << "Failed to initialize logging" << std::endl;
        return 1;
    }

    try {
        RunServer(settings.value("StatusServer/host").toString(), settings.value("StatusServer/port").toString());
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
