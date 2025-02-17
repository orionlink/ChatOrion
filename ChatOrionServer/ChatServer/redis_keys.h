//
// Created by hwk on 2025/2/14.
//

#ifndef REDIS_KEYS_H
#define REDIS_KEYS_H

#include <string>

namespace redis_keys
{
    // 消息详情 Hash结构
    inline std::string MessageKey(const std::string& msgId) {
        return "chat:msg:" + msgId;
    }

    // 用户消息列表 Sorted Set
    inline std::string UserMessagesKey(int uid) {
        return "chat:user_msgs:" + std::to_string(uid);
    }

    // 未读消息队列 List
    inline std::string UnreadKey(int toUid, int fromUid) {
        return "chat:unread:" + std::to_string(toUid) + ":" + std::to_string(fromUid);
    }

    // 会话关系 Hash
    inline std::string ChatRelationKey(int uid) {
        return "chat:relation:" + std::to_string(uid);
    }
}

#endif //REDIS_KEYS_H
