//
// Created by hwk on 25-8-20.
//

#ifndef ORION_MESSAGE_TEXT_CHAT_MESSAGE_H
#define ORION_MESSAGE_TEXT_CHAT_MESSAGE_H

#include "message_base.h"

namespace message
{
    /**
     * 聊天消息
     */
    class TextChatMessage : public MessageBase
    {
    public:
        TextChatMessage();

        std::string Serialize() override;

        bool Deserialize(const std::string& data) override;

        void set_chat_msg_id(const std::string& chat_msg_id) { _chat_msg_id = chat_msg_id; }
        void set_content(const std::string& content) { _content = content; }
        void set_msg_type(MessageType msg_type) { _msg_type = msg_type; }

        std::string chat_msg_id() const { return _chat_msg_id; }
        std::string content() const { return _content; }
        MessageType msg_type() const { return _msg_type; }
    private:
        std::string _chat_msg_id;
        std::string _content;
        MessageType _msg_type;
    };

}

#endif //ORION_MESSAGE_TEXT_CHAT_MESSAGE_H
