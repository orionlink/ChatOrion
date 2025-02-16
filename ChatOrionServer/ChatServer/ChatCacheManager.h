//
// Created by hwk on 2025/2/14.
//

#ifndef CHATCACHEMANAGER_H
#define CHATCACHEMANAGER_H

#include "singleton.hpp"
#include "const.h"
#include "data.h"

#include <vector>

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