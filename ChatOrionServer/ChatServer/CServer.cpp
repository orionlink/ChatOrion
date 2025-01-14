//
// Created by hwk on 2025/1/14.
//

#include "CServer.h"

#include <grpcpp/server.h>

#include "CSession.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context &context, unsigned short port)
    :_context(context), _port(port),_acceptor(context, tcp::endpoint(tcp::v4(), port))
{
    std::cout << "Server start success, listen on port : " << _port << std::endl;
    startAccept();
}

void CServer::startAccept()
{
    auto& context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(context, this);
    _acceptor.async_accept(new_session->get_socket(), std::bind(&CServer::handleAccept,
        this, new_session, std::placeholders::_1));
}

void CServer::handleAccept(std::shared_ptr<CSession> new_session, boost::system::error_code error_code)
{
    if (!error_code)
    {
        new_session->start();
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.insert(std::make_pair(new_session->get_uuid(), new_session));
    }
    else
    {
        std::cout << "session accept failed, error is " << error_code.what() << std::endl;
    }

    startAccept();
}

void CServer::clearSession(const std::string& uuid)
{
    if (_sessions.find(uuid) != _sessions.end())
    {
        //移除用户和session的关联
        // UserMgr::GetInstance()->RmvUserSession(_sessions[uuid]->GetUserId());
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.erase(uuid);
    }
}