#include <iostream>
#include <csignal>
#include <mutex>
#include <thread>

#include "AsioIOServicePool.h"
#include "Settings.h"
#include "const.h"
#include "CServer.h"

// ./ChatServer 8080
int main(int argc, char* argv[])
{
    try
    {
        auto &setting = config::Settings::GetInstance();
        setting.setFileName("config.ini");
        setting.load();

        unsigned short port;
        port = setting.value("ChatServer1/port").toInt();

        if (argc == 2)
        {
            port = std::atoi(argv[1]);
        }

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
