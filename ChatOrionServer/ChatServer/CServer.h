//
// Created by hwk on 2025/1/14.
//

#ifndef CSERVER_H
#define CSERVER_H

#include <boost/asio/ip/address.hpp>
#include <boost/asio/steady_timer.hpp>

#include "const.h"

class CSession;

using boost::asio::ip::tcp;

class CServer
{
public:
    CServer(boost::asio::io_context& context, const std::string service_name, unsigned short port);

    void clearSession(const std::string& session_id);
private:
    void startAccept();
    void startTimer();
    void onTimer(boost::system::error_code error);

    void handleAccept(std::shared_ptr<CSession> new_session, boost::system::error_code error_code);

    boost::asio::io_context& _context;
    unsigned short _port;
    tcp::acceptor _acceptor;
    std::map<std::string, std::shared_ptr<CSession>> _sessions;
    std::mutex _mutex;
    boost::asio::steady_timer _timer;

    std::string _service_name;
};

#endif //CSERVER_H
