//
// Created by hwk on 25-8-20.
//

#ifndef CHATSERVER_MESSAGEBASE_H
#define CHATSERVER_MESSAGEBASE_H

#include <memory>

#include "message_type.h"
#include "../utils.h"

namespace Json{
    class Value;
}

namespace message {

    /**
     * @brief 消息基类
     */
    class MessageBase {
    public:
        MessageBase() = default;
        virtual ~MessageBase() = default;

        virtual std::string Serialize() = 0;
        virtual bool Deserialize(const std::string& data) = 0;

        MSG_IDS message_id() const { return _message_id; }
        ErrorCodes error_codes() const { return _error_codes; }
        Timestamp timestamp() const { return _timestamp; }
        int from_uid() const { return _from_uid; }
        int to_uid() const { return _to_uid; }

        void set_from_uid(const int& id) { _from_uid = id; }
        void set_to_uid(const int& id) { _to_uid = id; }

    protected:
        bool ParseHeader(const std::string& data);

        void SeirializeHeader(Json::Value* data);

        MSG_IDS _message_id;
        ErrorCodes _error_codes;
        Timestamp _timestamp;
        int _from_uid;
        int _to_uid;
    };

}

#endif //CHATSERVER_MESSAGEBASE_H
