//
// Created by hwk on 2025/1/14.
//

#ifndef CSERVER_H
#define CSERVER_H

#include <boost/asio/ip/address.hpp>

#include "const.h"

class CSession;

using boost::asio::ip::tcp;

class CServer
{
public:
    CServer(boost::asio::io_context& context, unsigned short port);

    void clearSession(const std::string& uuid);
private:
    void startAccept();
    void handleAccept(std::shared_ptr<CSession> new_session, boost::system::error_code error_code);

    boost::asio::io_context& _context;
    unsigned short _port;
    tcp::acceptor _acceptor;
    std::map<std::string, std::shared_ptr<CSession>> _sessions;
    std::mutex _mutex;
};

#endif //CSERVER_H
