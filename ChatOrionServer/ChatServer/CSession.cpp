//
// Created by hwk on 2025/1/14.
//

#include "CSession.h"
#include "CServer.h"
#include "LogicSystem.h"

CSession::CSession(boost::asio::io_context &context, CServer *server)
    :_socket(context), _server(server), _b_head_parse(false), _b_close(false)
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    _session_id = boost::uuids::to_string(uuid);
    _recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

void CSession::start()
{
    asyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::asyncReadHead(int total_len)
{
    auto self = shared_from_this();
    asyncReadFull(total_len, [self, this](const boost::system::error_code& ec, std::size_t bytesTransfered)
    {
        try
        {
            if (ec)
            {
                std::cout << "handle read failed, error is " << ec.what() << std::endl;

                close();
                return;
            }

            if (bytesTransfered < HEAD_TOTAL_LEN)
            {
                std::cout << "read length not match, read [" << bytesTransfered << "] , total ["
                    << HEAD_TOTAL_LEN << "]" << std::endl;

                close();
                return;
            }

            _recv_head_node->clear();
            ::memcpy(_recv_head_node->_data, _data, bytesTransfered);

            //获取头部MSGID数据
            short msg_id = 0;
            memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
            //网络字节序转化为本地字节序
            msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
            std::cout << "msg_id is " << msg_id << std::endl;
            //id非法
            if (msg_id > MAX_LENGTH) {
                std::cout << "invalid msg_id is " << msg_id << std::endl;
                close();
                return;
            }
            short msg_len = 0;
            memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
            //网络字节序转化为本地字节序
            msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
            std::cout << "msg_len is " << msg_len << std::endl;

            //id非法
            if (msg_len > MAX_LENGTH) {
                std::cout << "invalid data length is " << msg_len << std::endl;
                close();
                return;
            }

            _recv_msg_node = std::make_shared<RecvNode>(msg_len, msg_id);
            asyncReadBody(msg_len);
        }
        catch (std::exception& e)
        {
            std::cout << "Exception code is " << e.what() << std::endl;
        }
    });
}

void CSession::asyncReadBody(int length)
{
    auto self = shared_from_this();
    asyncReadFull(length, [self, this, length](const boost::system::error_code& ec, std::size_t bytesTransfered)
    {
        try
        {
            if (ec)
            {
                std::cout << "handle read failed, error is " << ec.what() << std::endl;

                close();
                return;
            }

            if (bytesTransfered < length)
            {
                std::cout << "read length not match, read [" << bytesTransfered << "] , total ["
                    << HEAD_TOTAL_LEN << "]" << std::endl;

                close();
                return;
            }

            ::memcpy(_recv_msg_node->_data, _data, bytesTransfered);
            _recv_msg_node->_current_len += bytesTransfered;
            _recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';

            std::cout << "receive data is " << _recv_msg_node->_data << std::endl;
            //此处将消息投递到逻辑队列中
            LogicSystem::GetInstance()->postMsgToQue(std::make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
            //继续监听头部接受事件
            asyncReadHead(HEAD_TOTAL_LEN);
        }
        catch (std::exception& e)
        {
            std::cout << "Exception code is " << e.what() << std::endl;
        }
    });
}

void CSession::send(const char *msg, short msg_length, short msg_id)
{
    std::lock_guard<std::mutex> lock(_send_mutex);
    int queue_size = _send_queue.size();
    if (queue_size > MAX_SENDQUE)
    {
        std::cout << "session: " << _session_id << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    _send_queue.push(std::make_shared<SendNode>(msg, msg_length, msg_id));

    // 原本队列里面有元素，但是还没发送出去(handleWrite没有触发，没有pop出来), 就被其他线程push进来了
    // 不需要调用下面的发送，只需要在handleWrite按照队列发送即可
    if (queue_size > 0)
    {
        return;
    }

    auto& msgnode = _send_queue.front();
    boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
        std::bind(&CSession::handleWrite, this, std::placeholders::_1, sharedSelf()));
}

void CSession::send(const std::string &msg, short msg_id)
{
    std::lock_guard<std::mutex> lock(_send_mutex);
    int queue_size = _send_queue.size();
    if (queue_size > MAX_SENDQUE)
    {
        std::cout << "session: " << _session_id << " send que fulled, size is " << MAX_SENDQUE << std::endl;
        return;
    }

    _send_queue.push(std::make_shared<SendNode>(msg.c_str(), msg.length(), msg_id));

    if (queue_size > 0)
    {
        return;
    }

    auto& msgnode = _send_queue.front();
    boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
        std::bind(&CSession::handleWrite, this, std::placeholders::_1, sharedSelf()));
}

void CSession::close()
{
    _socket.close();
    _server->clearSession(_session_id);
    _b_close = true;
}

std::shared_ptr<CSession> CSession::sharedSelf()
{
    return shared_from_this();
}

void CSession::asyncReadFull(std::size_t maxLength,
                             std::function<void(const boost::system::error_code &, std::size_t)> handler)
{
    ::memset(_data, 0, MAX_LENGTH);
    asyncReadLen(0, maxLength, handler);
}

void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len,
    std::function<void(const boost::system::error_code &, std::size_t)> handler)
{
    auto self = shared_from_this();
    _socket.async_read_some(boost::asio::buffer(_data + read_len, total_len - read_len),
    [read_len, total_len, handler, self](const boost::system::error_code& ec, std::size_t bytesTransfered)
    {
        if (ec)
        {
            // 出现错误，调用回调函数
            handler(ec, read_len + bytesTransfered);
            return;
        }

        if (read_len + bytesTransfered >= total_len)
        {
            //长度够了就调用回调函数
            handler(ec, read_len + bytesTransfered);
            return;
        }

        // 没有错误，且长度不足则继续读取
        self->asyncReadLen(read_len + bytesTransfered, total_len, handler);
    });
}

void CSession::handleWrite(const boost::system::error_code &error, std::shared_ptr<CSession> shared_self)
{
    //增加异常处理
    try {
        if (!error)
        {
            std::lock_guard<std::mutex> lock(_send_mutex);
            //cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
            _send_queue.pop();
            if (!_send_queue.empty()) {
                auto& msgnode = _send_queue.front();
                boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
                    std::bind(&CSession::handleWrite, this, std::placeholders::_1, shared_self));
            }
        }
        else
        {
            std::cout << "handle write failed, error is " << error.what() << std::endl;
            close();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception code : " << e.what() << std::endl;
    }
}

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recv_node)
    :_session(session), _recv_node(recv_node)
{
}
