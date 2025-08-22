//
// Created by hwk on 25-8-21.
//

#ifndef ORION_MESSAGE_MESSAGE_FACTORY_H
#define ORION_MESSAGE_MESSAGE_FACTORY_H

#include "message/message_base.h"

#include <memory>
#include <unordered_map>
#include <functional>

namespace message {

    /**
     * @brief 消息工厂类，根据消息ID创建对应的消息对象
     */
    class MessageFactory {
    public:
        using MessageCreator = std::function<std::shared_ptr<message::MessageBase>()>;

        // 获取单例实例
        static MessageFactory& GetInstance() {
            static MessageFactory instance;
            return instance;
        }

        // 注册消息创建函数
        void RegisterProtocol(MSG_IDS msg_id, MessageCreator creator)
        {
            creators_[msg_id] = creator;
        }

        // 根据消息ID创建消息对象
        std::shared_ptr<MessageBase> createMessage(MSG_IDS msgId) {
            auto it = creators_.find(msgId);
            if (it != creators_.end()) {
                return it->second();
            }
            return nullptr;
        }

        // 创建消息并解析数据
        std::shared_ptr<MessageBase> ParseMessage(MSG_IDS msgId, const std::string& data) {
            auto msg = createMessage(msgId);
            if (msg && msg->Deserialize(data)) {
                return msg;
            }
            return nullptr;
        }

    private:
        MessageFactory() = default;
        std::unordered_map<MSG_IDS, MessageCreator> creators_;
    };

    // 注册器辅助类
    template<typename T>
    class MessageRegistrar {
    public:
        explicit MessageRegistrar(MSG_IDS msg_id) {
            MessageFactory::GetInstance().RegisterProtocol(msg_id, [] {
                return std::make_shared<T>();
            });
        }
    };

    // 方便注册的宏定义
#define REGISTER_MESSAGE(msgId, msgClass) \
        static message::MessageRegistrar<msgClass> msgClass##_registrar(msgId);

}

#endif //ORION_MESSAGE_MESSAGE_FACTORY_H
