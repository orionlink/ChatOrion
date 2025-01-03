#include "cserver.h"
#include "http_connection.h"
#include "AsioIOServicePool.h"

#include <iostream>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

CServer::CServer(boost::asio::io_context& ioc, unsigned short &port)
    :_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{
}

void CServer::start()
{
    std::cout << "CServer::start() work id: " << std::this_thread::get_id() << std::endl;
    auto self = shared_from_this();
    auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);
    _acceptor.async_accept(new_con->getSocket(), [self, new_con](beast::error_code err_code)
    {
        try
        {
            if (err_code)
            {
                self->start();
                return;
            }

            std::cout << "async_accept work id: " << std::this_thread::get_id() << std::endl;

            //处理新链接，创建HpptConnection类管理新连接
            new_con->start();
            //继续监听
            self->start();
        }
        catch (std::exception& exp)
        {
            std::cout << "exception is " << exp.what() << std::endl;
        }
    });
}
