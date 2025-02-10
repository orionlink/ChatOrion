//
// Created by hwk on 2025/2/10.
//

#ifndef CHATGRPCCLIENT_H
#define CHATGRPCCLIENT_H

#include "const.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "const.h"
#include "singleton.hpp"
#include "Settings.h"

#include <queue>
#include <grpcpp/grpcpp.h>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::AddFriendReq;
using message::AddFriendRsp;

using message::AuthFriendReq;
using message::AuthFriendRsp;

using message::GetChatServerRes;
using message::LoginRsp;
using message::LoginReq;
using message::ChatService;

using message::TextChatMsgReq;
using message::TextChatMsgRsp;
using message::TextChatData;

class ChatConPool
{
public:
    ChatConPool(size_t pool_size, std::string host, std::string port);

    std::unique_ptr<ChatService::Stub> getConnection();

    void returnConnection(std::unique_ptr<ChatService::Stub> context);

    void close();

private:
    size_t _pool_size;
    std::string _host;
    std::string _port;

    std::queue<std::unique_ptr<ChatService::Stub> > _connections;
    std::mutex _mutex;
    std::condition_variable _cond;
    std::atomic<bool> _b_stop;
};

class ChatGrpcClient : public Singleton<ChatGrpcClient>
{
public:
    ~ChatGrpcClient() {}

    /**
     * 通知对方加好友
     * @param server_ip
     * @param req
     * @return
     */
    AddFriendRsp NotifyAddFriend(std::string server_ip, const AddFriendReq& req);

    /**
     * 通知对方认证好友
     * @param server_ip
     * @param req
     * @return
     */
    AuthFriendRsp NotifyAuthFriend(std::string server_ip, const AuthFriendReq& req);
    bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);

    /**
     * 通知对方消息文本
     * @param server_ip
     * @param req
     * @param rtvalue
     * @return
     */
    TextChatMsgRsp NotifyTextChatMsg(std::string server_ip, const TextChatMsgReq& req, const Json::Value& rtvalue);
private:
    ChatGrpcClient();
    std::unordered_map<std::string, std::unique_ptr<ChatConPool>> _pools;
};



#endif //CHATGRPCCLIENT_H
