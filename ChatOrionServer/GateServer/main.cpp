#include <iostream>

#include "const.h"
#include "cserver.h"
#include  "Settings.h"

int main()
{
    try
    {
        auto &settings = config::Settings::GetInstance();
        settings.setFileName("config.ini");
        settings.load();

        unsigned short port = settings.value("GateServer/Port", 8080).toInt();
        net::io_context ioc{1};
        boost::asio::signal_set signal(ioc, SIGINT, SIGTERM);
        signal.async_wait([&ioc](boost::system::error_code err, int signal_number)
        {
            if (err) return;

            ioc.stop();
        });
        std::make_shared<CServer>(ioc, port)->start();
        std::cout << "Gate Server listen on port: " << port << std::endl;
        std::cout << "主线程 id: " << std::this_thread::get_id() << std::endl;
        ioc.run();
    }
    catch (std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
