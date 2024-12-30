#include "http_connection.h"

#include <iostream>

HttpConnection::HttpConnection(tcp::socket socket)
    :_socket(std::move(socket))
{

}

void HttpConnection::start()
{
    auto self = shared_from_this();
    http::async_read(_socket, _buffer, _request, [self](beast::error_code ec,
                     std::size_t bytes_transferred)
    {
        try
        {
            if (ec)
            {
                std::cout << "http read err is " << ec.what() << std::endl;
                return;
            }

            boost::ignore_unused(bytes_transferred);
            self->hanleReq();
            self->checkDeadline();
        }
        catch (std::exception& exp)
        {
            std::cout << "exception is " << exp.what() << std::endl;
        }
    });
}

void HttpConnection::hanleReq()
{
    _response.version(_request.version());
    _response.keep_alive(false);

    if (_request.method() == http::verb::get)
    {
        bool success; // = LogicSystem::GetInstance()->hanleGet(_request.target(), shared_from_this());
        if (!success)
        {
            _response.result(http::status::not_found);
            _response.set(http::field::content_type, "text/plain");
            beast::ostream(_response.body()) << "url not found\r\n";
            writeResponse();
            return;
        }

        _response.result(http::status::ok);
        _response.set(http::field::server, "GateServer");
        writeResponse();
        return;
    }
}

void HttpConnection::checkDeadline()
{
    auto self = shared_from_this();

    _deadline.async_wait(
        [self](beast::error_code ec)
        {
            if (!ec)
            {
                // Close socket to cancel any outstanding operation.
                self->_socket.close(ec);
            }
        });
}

void HttpConnection::writeResponse()
{
    auto self = shared_from_this();
    _response.content_length(_response.body().size());
    http::async_write(_socket, _response, [self](beast::error_code ec,
                      std::size_t)
    {

        self->_socket.shutdown(tcp::socket::shutdown_send, ec);
        self->_deadline.cancel();
    });
}
