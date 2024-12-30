#ifndef CSERVER_H
#define CSERVER_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

class CServer : public std::enable_shared_from_this<CServer>
{
public:
    CServer(boost::asio::io_context& ioc, unsigned short &port);
    void start();
private:
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::io_context& _ioc;
    boost::asio::ip::tcp::socket _socket;
};

#endif // CSERVER_H
