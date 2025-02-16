//
// Created by hwk on 2025/2/10.
//

#ifndef DATA_H
#define DATA_H

#include <string>
#include <utility> // std::move
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

struct UserInfo
{
    UserInfo():name(""), pwd(""),uid(0),email(""),nick(""),desc(""),sex(0), icon(""), back("") {}
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
    std::string nick;
    std::string desc;
    int sex;
    std::string icon;
    std::string back;
};

struct ApplyInfo
{
    ApplyInfo(int uid, std::string name, std::string desc,
        std::string icon, std::string nick, int sex, int status)
        :_uid(uid),_name(name),_desc(desc),
        _icon(icon),_nick(nick),_sex(sex),_status(status){}

    int _uid;
    std::string _name;
    std::string _desc;
    std::string _icon;
    std::string _nick;
    int _sex;
    int _status;
};

enum class MessageType {
    TEXT = 1,
    IMAGE = 2,
    VIDEO = 3,
};

struct ChatMessage
{
    std::string msg_id;
    int from_uid;
    int to_uid;
    std::string content;
    int msg_type;
    int64_t send_time;
    int status;  // 0: 未读, 1: 已读, 2: 已删除

    // 默认构造函数
    ChatMessage()
        : from_uid(0), to_uid(0), msg_type(1), send_time(0), status(0) {}

    // 带参数的构造函数
    ChatMessage(std::string msg_id, int from_uid, int to_uid,
                std::string content, int msg_type, time_t send_time, int status)
        : msg_id(std::move(msg_id)), from_uid(from_uid), to_uid(to_uid),
          content(std::move(content)), msg_type(msg_type),
          send_time(send_time), status(status) {}

    // 拷贝构造函数
    ChatMessage(const ChatMessage& other)
        : msg_id(other.msg_id), from_uid(other.from_uid), to_uid(other.to_uid),
          content(other.content), msg_type(other.msg_type),
          send_time(other.send_time), status(other.status) {}

    // 移动构造函数
    ChatMessage(ChatMessage&& other) noexcept
        : msg_id(std::move(other.msg_id)), from_uid(other.from_uid), to_uid(other.to_uid),
          content(std::move(other.content)), msg_type(other.msg_type),
          send_time(other.send_time), status(other.status) {}

    // 拷贝赋值运算符
    ChatMessage& operator=(const ChatMessage& other) {
        if (this != &other) {
            msg_id = other.msg_id;
            from_uid = other.from_uid;
            to_uid = other.to_uid;
            content = other.content;
            msg_type = other.msg_type;
            send_time = other.send_time;
            status = other.status;
        }
        return *this;
    }

    // 移动赋值运算符
    ChatMessage& operator=(ChatMessage&& other) noexcept {
        if (this != &other) {
            msg_id = std::move(other.msg_id);
            from_uid = other.from_uid;
            to_uid = other.to_uid;
            content = std::move(other.content);
            msg_type = other.msg_type;
            send_time = other.send_time;
            status = other.status;
        }
        return *this;
    }

    // JSON 序列化
    Json::Value toJson() const {
        Json::Value json;
        json["msg_id"] = msg_id;
        json["from_uid"] = from_uid;
        json["to_uid"] = to_uid;
        json["content"] = content;
        json["msg_type"] = msg_type;
        json["send_time"] = send_time;
        json["status"] = status;
        return json;
    }

    void parseJson(const Json::Value& json)
    {
        if (!json.isObject()) return;  // 处理无效 JSON

        msg_id = json.get("msg_id", "").asString();
        from_uid = json.get("from_uid", 0).asInt();
        to_uid = json.get("to_uid", 0).asInt();
        content = json.get("content", "").asString();
        msg_type = json.get("msg_type", 1).asInt();
        send_time = json.get("send_time", 0).asInt64();  // 64 位整数转换为 time_t
        status = json.get("status", 0).asInt();
    }

    // JSON 反序列化
    static ChatMessage fromJson(const Json::Value& json) {
        ChatMessage msg;
        if (!json.isObject()) return msg;  // 处理无效 JSON

        msg.msg_id = json.get("msg_id", "").asString();
        msg.from_uid = json.get("from_uid", 0).asInt();
        msg.to_uid = json.get("to_uid", 0).asInt();
        msg.content = json.get("content", "").asString();
        msg.msg_type = json.get("msg_type", 1).asInt();
        msg.send_time = json.get("send_time", 0).asInt64();  // 64 位整数转换为 time_t
        msg.status = json.get("status", 0).asInt();
        return msg;
    }
};


#endif //DATA_H
