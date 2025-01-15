#include <iostream>
#include <csignal>
#include <mutex>
#include <thread>

#include "AsioIOServicePool.h"
#include "Settings.h"
#include "const.h"
#include "CServer.h"
#include "RedisManager.h"

static void use(char* argv[])
{
    std::cout << argv[0] << " arg1: 服务器标识, argc2: 端口号" << std::endl;
    std::cout << "示例: " <<  argv[0] << " 1 8080"<< std::endl;
}

// ./ChatServer 1 8080, 1 为服务器id标识, 拼接字符串以后就是 ChatServer1
int main(int argc, char* argv[])
{
    use(argv);

    try
    {
        auto &setting = config::Settings::GetInstance();
        setting.setFileName("config.ini");
        setting.load();

        unsigned short port;
        port = setting.value("ChatServer1/port").toInt();

        std::string server_name = "ChatServer";
        std::string server_name_id = "";
        if (argc == 3)
        {
            server_name_id = argv[1];
            port = std::atoi(argv[2]);
        }
        server_name += server_name_id;
        std::cout << "server_name: " << server_name << std::endl;

        // 将登录数设置为0
        RedisManager::GetInstance()->hset(LOGIN_COUNT, server_name, "0");

        auto pool = AsioIOServicePool::GetInstance();
        boost::asio::io_context context;
        boost::asio::signal_set signals(context, SIGINT, SIGTERM);
        signals.async_wait([&context, &pool](auto, auto)
        {
            context.stop();
            pool->stop();
        });

        CServer s(context, port);
        context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
