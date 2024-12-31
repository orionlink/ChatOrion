#include <iostream>

#include "const.h"
#include "cserver.h"

int main()
{
    try
    {
        unsigned short port = static_cast<unsigned short>(8080);
        net::io_context ioc{1};
        boost::asio::signal_set signal(ioc, SIGINT, SIGTERM);
        signal.async_wait([&ioc](boost::system::error_code err, int signal_number)
        {
            if (err) return;

            ioc.stop();
        });
        std::make_shared<CServer>(ioc, port)->start();
        std::cout << "Gate Server listen on port: " << port << std::endl;
        ioc.run();
    }
    catch (std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
