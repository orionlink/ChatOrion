//
// Created by hwk on 2025/2/14.
//

#include "ChatCacheManager.h"
#include "RedisManager.h"
#include "const.h"
#include "redis_keys.h"
#include "log.h"
#include "MySQLManager.h"

// 缓存新消息
bool ChatCacheManager::CacheNewMessage(int fromUid, int toUid, const std::string& msgId,
                    const std::string& content, int msgType)
{
    try {
        auto redis = RedisManager::GetInstance();

        // 1. 存储消息详情
        std::string msgKey = redis_keys::MessageKey(msgId);
        if (!redis->hset(msgKey, "from_uid", std::to_string(fromUid)) ||
            !redis->hset(msgKey, "to_uid", std::to_string(toUid)) ||
            !redis->hset(msgKey, "content", content) ||
            !redis->hset(msgKey, "msg_type", std::to_string(msgType)) ||
            !redis->hset(msgKey, "timestamp", std::to_string(time(nullptr))) ||
            !redis->hset(msgKey, "status", "0")) {
            LOG_ERROR << "Failed to save message details to Redis";
            return false;
        }

        // 设置7天过期
        redis->expire(msgKey, 7 * 24 * 3600);

        // 2. 添加到用户消息列表
        std::string score = std::to_string(time(nullptr));
        std::string fromMsgsKey = redis_keys::UserMessagesKey(fromUid);
        std::string toMsgsKey = redis_keys::UserMessagesKey(toUid);

        // 使用原生ZADD命令
        std::string zaddCmd1 = "ZADD " + fromMsgsKey + " " + score + " " + msgId;
        std::string zaddCmd2 = "ZADD " + toMsgsKey + " " + score + " " + msgId;

        redis->execute(zaddCmd1);
        redis->execute(zaddCmd2);

        // 3. 添加到未读消息队列
        std::string unreadKey = redis_keys::UnreadKey(toUid, fromUid);
        redis->rpush(unreadKey, msgId);

        // 4. 更新会话关系和未读计数
        std::string relationKey = redis_keys::ChatRelationKey(toUid);

        // 直接使用hset而不是HINCRBY
        std::string unreadCount;
        if (redis->hget(relationKey, std::to_string(fromUid), unreadCount)) {
            // 如果已存在，递增
            int count = 1;
            try {
                count = std::stoi(unreadCount) + 1;
                if (count < 1) count = 1;  // 确保计数至少为1
            } catch (...) {
                count = 1;  // 如果转换失败，设为1
            }
            redis->hset(relationKey, std::to_string(fromUid), std::to_string(count));
        } else {
            // 如果不存在，设置为1
            redis->hset(relationKey, std::to_string(fromUid), "1");
        }

        return true;
    } catch (const std::exception& e) {
        LOG_ERROR << "Cache message failed: " << e.what();
        return false;
    }
}

bool ChatCacheManager::CacheNewMessage(const ChatMessage& msg)
{
    return CacheNewMessage(msg.from_uid, msg.to_uid, msg.msg_id, msg.content, msg.msg_type);
}

// 获取最近消息
std::vector<ChatMessage> ChatCacheManager::GetRecentMessages(int uid, int limit)
{
    std::vector<ChatMessage> messages;
    try {
        auto redis = RedisManager::GetInstance();
        std::string msgsKey = redis_keys::UserMessagesKey(uid);

        // 1. 获取消息ID列表
        auto msgIds = redis->zrevrange(msgsKey, 0, limit - 1);

        // 2. 获取每条消息的详细信息
        for (const auto& msgId : msgIds) {
            std::string msgKey = redis_keys::MessageKey(msgId);

            ChatMessage msg;
            msg.msg_id = msgId;

            std::string value;
            if (redis->hget(msgKey, "from_uid", value))
                msg.from_uid = std::stoi(value);
            if (redis->hget(msgKey, "to_uid", value))
                msg.to_uid = std::stoi(value);
            if (redis->hget(msgKey, "content", value))
                msg.content = value;
            if (redis->hget(msgKey, "msg_type", value))
                msg.msg_type = std::stoi(value);
            if (redis->hget(msgKey, "timestamp", value))
                msg.send_time = std::stol(value);
            if (redis->hget(msgKey, "status", value))
                msg.status = std::stoi(value);

            messages.push_back(msg);
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Get recent messages failed: " << e.what();
    }
    return messages;
}

// 标记消息已读
bool ChatCacheManager::MarkMessagesAsRead(int uid, int peerId)
{
    try {
        auto redis = RedisManager::GetInstance();

        // 1. 清空未读消息队列
        std::string unreadKey = redis_keys::UnreadKey(uid, peerId);
        redis->del(unreadKey);

        // 2. 重置未读计数
        std::string relationKey = redis_keys::ChatRelationKey(uid);
        redis->hset(relationKey, std::to_string(peerId), "0");

        return true;
    } catch (const std::exception& e) {
        LOG_ERROR << "Mark messages as read failed: " << e.what();
        return false;
    }
}

// 获取未读消息
std::vector<ChatMessage> ChatCacheManager::GetUnreadMessages(int uid)
{
    std::vector<ChatMessage> messages;
    try {
        auto redis = RedisManager::GetInstance();

        // 1. 从关系表中获取未读消息数
        std::string relationKey = redis_keys::ChatRelationKey(uid);
        std::vector<std::string> peerIds;  // 存储所有发送者的ID

        LOG_INFO << "Getting unread messages for uid: " << uid << ", relation key: " << relationKey;

        // 获取所有的peer ids
        if (!redis->hkeys(relationKey, peerIds)) {
            LOG_ERROR << "Failed to get peer ids from relation key: " << relationKey;
            return messages;
        }

        LOG_INFO << "Found " << peerIds.size() << " peers in relation";

        // 遍历每个对话
        for (const auto& peerId : peerIds) {
            std::string unreadCount;
            if (redis->hget(relationKey, peerId, unreadCount)) {
                LOG_INFO << "Peer " << peerId << " has unread count: " << unreadCount;

                try {
                    if (std::stoi(unreadCount) > 0) {
                        // 2. 获取该对话的未读消息
                        std::string unreadKey = redis_keys::UnreadKey(uid, std::stoi(peerId));
                        LOG_INFO << "Fetching messages from unread key: " << unreadKey;

                        std::vector<std::string> msgIds;
                        if (redis->lrange(unreadKey, msgIds)) {
                            LOG_INFO << "Found " << msgIds.size() << " unread messages";

                            // 3. 获取每条消息的详情
                            for (const auto& msgId : msgIds) {
                                std::string msgKey = redis_keys::MessageKey(msgId);
                                LOG_INFO << "Getting message details for: " << msgKey;

                                ChatMessage msg;
                                msg.msg_id = msgId;

                                std::string value;
                                if (redis->hget(msgKey, "from_uid", value))
                                    msg.from_uid = std::stoi(value);
                                if (redis->hget(msgKey, "to_uid", value))
                                    msg.to_uid = std::stoi(value);
                                if (redis->hget(msgKey, "content", value))
                                    msg.content = value;
                                if (redis->hget(msgKey, "msg_type", value))
                                    msg.msg_type = std::stoi(value);
                                if (redis->hget(msgKey, "timestamp", value))
                                    msg.send_time = std::stol(value);
                                if (redis->hget(msgKey, "status", value))
                                    msg.status = std::stoi(value);

                                messages.push_back(msg);
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    LOG_ERROR << "Error processing unread count for peer " << peerId << ": " << e.what();
                    continue;  // 继续处理下一个peer
                }
            }
        }

        LOG_INFO << "Total unread messages found: " << messages.size();

    } catch (const std::exception& e) {
        LOG_ERROR << "Get unread messages failed: " << e.what();
    }
    return messages;
}
