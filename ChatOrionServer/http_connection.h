#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include "const.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;
public:
    HttpConnection(tcp::socket socket);
    void start();
private:
    void hanleReq();
    void checkDeadline();
    void writeResponse();

private:
    tcp::socket _socket;

    beast::flat_buffer _buffer{ 8192 };
    http::request<http::dynamic_body> _request;
    http::response<http::dynamic_body> _response;
    net::steady_timer _deadline{ _socket.get_executor(), std::chrono::seconds(60) };
};

#endif // HTTPCONNECTION_H
