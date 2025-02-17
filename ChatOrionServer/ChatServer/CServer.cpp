//
// Created by hwk on 2025/1/14.
//



#include <grpcpp/server.h>

#include "CSession.h"
#include "AsioIOServicePool.h"
#include "UserMgr.h"
#include "CServer.h"
#include "log.h"
#include "StatusGrpcClient.h"

CServer::CServer(boost::asio::io_context &context, const std::string service_name, unsigned short port)
    :_context(context), _port(port),_acceptor(context, tcp::endpoint(tcp::v4(), port)), _timer(context), _service_name(service_name)
{
    LOG_INFO << "Server start success, listen on port : " << _port;
    startAccept();
    startTimer();
}

void CServer::startAccept()
{
    auto& context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<CSession> new_session = std::make_shared<CSession>(context, this);
    _acceptor.async_accept(new_session->get_socket(), std::bind(&CServer::handleAccept,
        this, new_session, std::placeholders::_1));
}

void CServer::startTimer()
{
    _timer.expires_after(boost::asio::chrono::seconds(15));
    _timer.async_wait(std::bind(&CServer::onTimer, this,
        std::placeholders::_1));
}

void CServer::onTimer(boost::system::error_code error)
{
    if (!error)
    {
        MessageRes message_res = StatusGrpcClient::GetInstance()->Heartbeat(_service_name);
        if (message_res.error() != message::Success)
        {
            LOG_ERROR << "心跳发送失败";
            if (message_res.error() == message::ServerNotFound)
            {
                LOG_INFO << "注册被断开，开始自动重连";
                StatusGrpcClient::GetInstance()->RegisterChatServer(_service_name, "", std::to_string(_port));
            }
        }
        startTimer();  // 重新启动定时器
    }
}

void CServer::handleAccept(std::shared_ptr<CSession> new_session, boost::system::error_code error_code)
{
    if (!error_code)
    {
        new_session->start();
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.insert(std::make_pair(new_session->get_session_id(), new_session));
    }
    else
    {
        LOG_ERROR << "session accept failed, error is " << error_code.what() << std::endl;
    }

    startAccept();
}

void CServer::clearSession(const std::string& session_id)
{
    if (_sessions.find(session_id) != _sessions.end())
    {
        //移除用户和session的关联
        UserMgr::GetInstance()->RemoveUserSession(_sessions[session_id]->get_user_id());
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _sessions.erase(session_id);
    }
}