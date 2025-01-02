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
    /**
     * @brief 处理带参数的url
     * @param url 未处理过的url
     * @return
     */
    std::string preParseGetParam(const std::string& url);
private:
    tcp::socket _socket;

    beast::flat_buffer _buffer{ 8192 };
    http::request<http::dynamic_body> _request;
    http::response<http::dynamic_body> _response;
    net::steady_timer _deadline{ _socket.get_executor(), std::chrono::seconds(60) };

    std::string _get_url;
    std::unordered_map<std::string, std::string> _get_params;
};

#endif // HTTPCONNECTION_H
