//
// Created by hwk on 25-8-20.
//

#include "message_base.h"

#include <json/json.h>

namespace message
{
    bool MessageBase::ParseHeader(const std::string &data) {
        Json::Value root;
        Json::Reader reader;
        if (reader.parse(data, root))
        {
            _message_id = static_cast<MSG_IDS>(root["message_id"].asInt());
            _error_codes = static_cast<ErrorCodes>(root["error_codes"].asInt());
            _timestamp = Timestamp::from_seconds(root["timestamp"].asUInt64());
            _from_uid = root["from_uid"].asInt();
            _to_uid = root["to_uid"].asInt();
        }
        return false;
    }

    void MessageBase::SeirializeHeader(Json::Value *data) {
        if (data == nullptr) return;
        auto root = *data;
        root["message_id"] = _message_id;
        root["error_codes"] = _error_codes;
        root["timestamp"] = Json::Value(static_cast<Json::UInt64>(_timestamp.to_seconds()));
        root["message_id"] = _from_uid;
        root["message_id"] = _to_uid;
    }
}
