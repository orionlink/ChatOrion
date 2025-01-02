#include "cserver.h"
#include "http_connection.h"

#include <iostream>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

CServer::CServer(boost::asio::io_context& ioc, unsigned short &port)
    :_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _socket(ioc)
{
}

void CServer::start()
{
    auto self = shared_from_this();
    _acceptor.async_accept(_socket, [self](beast::error_code err_code)
    {
        try
        {
            if (err_code)
            {
                self->start();
                return;
            }

            //处理新链接，创建HpptConnection类管理新连接
            std::make_shared<HttpConnection>(std::move(self->_socket))->start();
            //继续监听
            self->start();
        }
        catch (std::exception& exp)
        {
            std::cout << "exception is " << exp.what() << std::endl;
        }
    });
}
