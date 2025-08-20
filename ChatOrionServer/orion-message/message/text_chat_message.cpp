//
// Created by hwk on 25-8-20.
//

#include "text_chat_message.h"

#include <json/json.h>

namespace message
{
    TextChatMessage::TextChatMessage() {
        _message_id = ID_TEXT_CHAT_MSG_REQ;
    }

    std::string TextChatMessage::Serialize() {
        Json::Value root;
        MessageBase::SeirializeHeader(&root);
        Json::FastWriter fast_writer;
        root["msg_type"] = _msg_type;
        root["chat_msg_id"] = _chat_msg_id;
        root["content"] = _content;

        auto serialize_str = fast_writer.write(root);

        return serialize_str;
    }

    bool TextChatMessage::Deserialize(const std::string &data) {
        if (!MessageBase::ParseHeader(data)) return false;
        Json::Value root;
        Json::Reader reader;
        if (reader.parse(data, root))
        {
            _msg_type = static_cast<MessageType>(root["msg_type"].asInt());
            _chat_msg_id = root["chat_msg_id"].asString();
            _content = root["content"].asString();
        }

        return false;
    }
}
