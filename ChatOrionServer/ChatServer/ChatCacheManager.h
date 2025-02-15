//
// Created by hwk on 2025/2/14.
//

#ifndef CHATCACHEMANAGER_H
#define CHATCACHEMANAGER_H

#include "singleton.hpp"
#include "const.h"

#include <vector>

struct ChatMessage {
    std::string msg_id;
    int from_uid;
    int to_uid;
    std::string content;
    int msg_type;
    time_t send_time;
    int status;  // 0: 未读, 1: 已读, 2: 已删除

    ChatMessage() : from_uid(0), to_uid(0), msg_type(1),
                   send_time(0), status(0) {}

    Json::Value toJson() const {
        Json::Value json;
        json["msg_id"] = msg_id;
        json["from_uid"] = from_uid;
        json["to_uid"] = to_uid;
        json["content"] = content;
        json["msg_type"] = msg_type;
        json["send_time"] = static_cast<Json::Int64>(send_time);
        json["status"] = status;
        return json;
    }

    static ChatMessage fromJson(const Json::Value& json) {
        ChatMessage msg;
        msg.msg_id = json["msg_id"].asString();
        msg.from_uid = json["from_uid"].asInt();
        msg.to_uid = json["to_uid"].asInt();
        msg.content = json["content"].asString();
        msg.msg_type = json["msg_type"].asInt();
        msg.send_time = json["send_time"].asInt64();
        msg.status = json["status"].asInt();
        return msg;
    }
};

class ChatCacheManager : public Singleton<ChatCacheManager>
{
    friend class Singleton<ChatCacheManager>;
public:
    bool CacheNewMessage(int fromUid, int toUid, const std::string& msgId,
                        const std::string& content, int msgType = 1);

    bool CacheNewMessage(const ChatMessage& msg);

    // 获取最近消息
    std::vector<ChatMessage> GetRecentMessages(int uid, int limit = 50);

    // 获取未读消息
    std::vector<ChatMessage> GetUnreadMessages(int uid);

    // 标记消息已读
    bool MarkMessagesAsRead(int uid, int peerId);

private:
    ChatCacheManager() = default;

    static constexpr int CACHE_EXPIRE_TIME = 7 * 24 * 3600;  // 7天过期
    static constexpr int MAX_CACHED_MESSAGES = 50;  // 最多缓存50条消息
};

#endif //CHATCACHEMANAGER_H