#include "http_connection.h"
#include "logic_system.h"
#include "tools.h"

#include <iostream>

HttpConnection::HttpConnection(boost::asio::io_context& ioc)
    :_socket(ioc)
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

            std::cout << "async_read work id: " << std::this_thread::get_id() << std::endl;

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
        std::string result_url =  preParseGetParam(_request.target());
        bool success = LogicSystem::GetInstance()->handleGet(result_url, shared_from_this());
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
    else if(_request.method() == http::verb::post)
    {
        std::string path = _request.target();
        std::cout << "post path: " << path << std::endl;
        bool success = LogicSystem::GetInstance()->handlePost(path, shared_from_this());
        if (!success) {
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

std::string HttpConnection::preParseGetParam(const std::string &url)
{
    // http://localhost:8080/get_test?key1=value1&key2=value2
    // url = get_test?key1=value1&key2=value2
    std::string result_url;
    // 查找查询字符串的开始位置（即 '?' 的位置）
    auto query_pos = url.find('?');
    if (query_pos == std::string::npos) {
        result_url = url;
        return result_url;
    }

    result_url = url.substr(0, query_pos);
    std::string query_string = url.substr(query_pos + 1);
    std::string key;
    std::string value;
    size_t pos = 0;
    while ((pos = query_string.find('&')) != std::string::npos) {
        auto pair = query_string.substr(0, pos);
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            // 可能带有中文, 需要decode
            key = Tools::UrlDecode(pair.substr(0, eq_pos)); // 假设有 url_decode 函数来处理URL解码
            value = Tools::UrlDecode(pair.substr(eq_pos + 1));
            _get_params[key] = value;
        }
        query_string.erase(0, pos + 1);
    }
    // 处理最后一个参数对（如果没有 & 分隔符）
    if (!query_string.empty()) {
        size_t eq_pos = query_string.find('=');
        if (eq_pos != std::string::npos) {
            key = Tools::UrlDecode(query_string.substr(0, eq_pos));
            value = Tools::UrlDecode(query_string.substr(eq_pos + 1));
            _get_params[key] = value;
        }
    }

    return result_url;
}
