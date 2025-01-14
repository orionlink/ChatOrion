//
// Created by hwk on 2025/1/14.
//

#ifndef CSESSION_H
#define CSESSION_H

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <queue>
#include <mutex>
#include <memory>

#include "const.h"
#include "MsgNode.h"

class CServer;

class CSession : public std::enable_shared_from_this<CSession>
{
public:
    CSession(boost::asio::io_context& context, CServer* server);

    void start();

    void asyncReadHead(int total_len);
    void asyncReadBody(int length);

    void close();

    boost::asio::ip::tcp::socket& get_socket() { return _socket; }

    std::string get_uuid() { return _session_id; }
private:
    /**
     * 读取完整长度
     * @param maxLength 读取的最大长度
     * @param handler 读取完成后的处理函数，包含错误处理
     */
    void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler);

    /**
     * 读取指定字节数
     * @param read_len 已经读取的长度
     * @param total_len 读取的总长度
     * @param handler 读取完成后的处理函数，包含错误处理
     */
    void asyncReadLen(std::size_t read_len, std::size_t total_len,
                      std::function<void(const boost::system::error_code&, std::size_t)> handler);
private:
    tcp::socket _socket;
    std::string _session_id;
    CServer *_server;

    char _data[MAX_LENGTH];

    bool _b_close;

    //收到的消息结构
    std::shared_ptr<RecvNode> _recv_msg_node;
    bool _b_head_parse;

    //收到的头部结构
    std::shared_ptr<MsgNode> _recv_head_node;
};

class LogicNode {
    friend class LogicSystem;
public:
    LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<RecvNode> recv_node);
private:
    std::shared_ptr<CSession> _session;
    std::shared_ptr<RecvNode> _recv_node;
};

#endif //CSESSION_H
