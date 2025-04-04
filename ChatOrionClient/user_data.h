#ifndef USERINFO_H
#define USERINFO_H

#include <QString>
#include <memory>
#include <QJsonArray>
#include <QJsonObject>
#include <vector>
#include <QObject>

#include "singleton.h"

class SearchInfo
{
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon);
    int _uid{0};
    QString _name{""};
    QString _nick{""};
    QString _desc{""};
    int _sex{0};
    QString _icon{""};
};

class AddFriendApply
{
public:
    AddFriendApply(int from_uid, QString name, QString desc,
                   QString icon, QString nick, int sex);
    int _from_uid{0};
    QString _name{""};
    QString _desc{""};
    QString _icon{""};
    QString _nick{""};
    int     _sex{0};
};

struct ApplyInfo
{
    ApplyInfo(int uid, QString name, QString desc,
        QString icon, QString nick, int sex, int status)
        :_uid(uid),_name(name),_desc(desc),
        _icon(icon),_nick(nick),_sex(sex),_status(status){}

    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
        :_uid(addinfo->_from_uid),_name(addinfo->_name),
          _desc(addinfo->_desc),_icon(addinfo->_icon),
          _nick(addinfo->_nick),_sex(addinfo->_sex),
          _status(0)
    {}
    void SetIcon(QString head)
    {
        _icon = head;
    }
    int _uid{0};
    QString _name{""};
    QString _desc{""};
    QString _icon{""};
    QString _nick{""};
    int _sex{0};
    int _status{0};
};

struct AuthInfo
{
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int sex):
        _uid(uid), _name(name), _nick(nick), _icon(icon),
        _sex(sex){}
    int _uid{0};
    QString _name{""};
    QString _nick{""};
    QString _icon{""};
    int _sex{0};
};

struct AuthRsp
{
    AuthRsp(int peer_uid, QString peer_name,
            QString peer_nick, QString peer_icon, int peer_sex)
        :_uid(peer_uid),_name(peer_name),_nick(peer_nick),
          _icon(peer_icon),_sex(peer_sex)
    {}

    int _uid{0};
    QString _name{""};
    QString _nick{""};
    QString _icon{""};
    int _sex{0};
};

struct TextChatData;
struct FriendInfo
{
    FriendInfo(int uid, QString name, QString nick, QString icon,
        int sex, QString desc, QString back, QString last_msg="", int64_t last_msg_time = 0):_uid(uid),
        _name(name),_nick(nick),_icon(icon),_sex(sex),_desc(desc),
        _back(back),_last_msg(last_msg), _last_msg_time(last_msg_time){}

    FriendInfo(std::shared_ptr<AuthInfo> auth_info):_uid(auth_info->_uid),
    _nick(auth_info->_nick),_icon(auth_info->_icon),_name(auth_info->_name),
      _sex(auth_info->_sex){}

    FriendInfo(std::shared_ptr<AuthRsp> auth_rsp):_uid(auth_rsp->_uid),
    _nick(auth_rsp->_nick),_icon(auth_rsp->_icon),_name(auth_rsp->_name),
      _sex(auth_rsp->_sex){}

    void AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec);

    int _uid{0};
    QString _name{""};
    QString _nick{""};
    QString _icon{""};
    int _sex{0};
    QString _desc{""};
    QString _back;
    QString _last_msg{""};
    int64_t _last_msg_time{0};
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};


struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "", int64_t last_msg_time = 0):
        _uid(uid),_name(name),_nick(nick),_icon(icon),_sex(sex),_last_msg(last_msg), _last_msg_time(last_msg_time){}

    UserInfo(std::shared_ptr<AuthInfo> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_last_msg(""){}

    UserInfo(int uid, QString name, QString icon):
    _uid(uid), _name(name), _icon(icon),_nick(_name),
    _sex(0),_last_msg(""){

    }

    UserInfo(std::shared_ptr<AuthRsp> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_last_msg(""){}

    UserInfo(std::shared_ptr<SearchInfo> search_info):
        _uid(search_info->_uid),_name(search_info->_name),_nick(search_info->_nick),
    _icon(search_info->_icon),_sex(search_info->_sex),_last_msg(""){

    }

    UserInfo(std::shared_ptr<FriendInfo> friend_info):
        _uid(friend_info->_uid),_name(friend_info->_name),_nick(friend_info->_nick),
        _icon(friend_info->_icon),_sex(friend_info->_sex),_last_msg(friend_info->_last_msg),
        _last_msg_time(friend_info->_last_msg_time)
    {
            _chat_msgs = friend_info->_chat_msgs;
    }

    int _uid{0};
    QString _name{""};
    QString _nick{""};
    QString _icon{""};
    int _sex{0};
    QString _last_msg{""};
    int64_t _last_msg_time{0};
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

struct TextChatData
{
    TextChatData(QString msg_id, QString msg_content, int fromuid, int touid
                 , int64_t send_time, int msg_type, int status)
        :_msg_id(msg_id),_msg_content(msg_content),_from_uid(fromuid),_to_uid(touid)
        , _send_time(send_time), _msg_type(msg_type), _status(status)
    {

    }
    QString _msg_id;
    QString _msg_content;
    int _from_uid;
    int _to_uid;
    int64_t _send_time;
    int _msg_type;
    int _status;
};

struct TextChatMsg
{
    TextChatMsg(int fromuid, int touid, const QString& msgid,  const QString& content
                , int64_t send_time, int msg_type, int status)
        :_from_uid(fromuid),_to_uid(touid)
    {
        auto msg_ptr = std::make_shared<TextChatData>(msgid, content,fromuid, touid, send_time, msg_type, status);
        _chat_msgs.push_back(msg_ptr);
    }

    int _from_uid;
    int _to_uid;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

class EmojiManager : public Singleton<EmojiManager>
{
    friend class Singleton<EmojiManager>;
public:
    bool loadEmojiData(const QString& fileName);

    QJsonObject getEmojiObject() const {
        return m_emojiObject;
    }

    ~EmojiManager(){}
private:
    EmojiManager(){}


    QJsonObject m_emojiObject;
};

#endif // USERINFO_H
