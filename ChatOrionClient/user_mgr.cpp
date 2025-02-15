#include "user_mgr.h"

#include <QJsonArray>
#include <QDebug>

UserMgr::UserMgr():_user_info(nullptr), _contact_loaded(0), _chat_loaded(0),_chat_count_per_page(13)
{

}

UserMgr::~UserMgr()
{

}

void UserMgr::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
}

void UserMgr::SetToken(QString token)
{
    _token = token;
}

int UserMgr::GetUid()
{
    return _user_info->_uid;
}

QString UserMgr::GetName()
{
    return _user_info->_name;
}

QString UserMgr::GetIcon()
{
    return _user_info->_icon;
}

std::shared_ptr<UserInfo> UserMgr::GetUserInfo()
{
    return _user_info;
}

void UserMgr::AppendFriendList(QJsonArray array)
{
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue& value : array) {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto back = value["back"].toString();

        auto info = std::make_shared<FriendInfo>(uid, name,
            nick, icon, sex, desc, back);
        _friend_list.push_back(info);
        _friend_map.insert(uid, info);
    }
}

bool UserMgr::CheckFriendById(int uid)
{
    auto iter = _friend_map.find(uid);
    if(iter == _friend_map.end()){
        return false;
    }

    return true;
}

void UserMgr::AddFriend(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
    _friend_map[friend_info->_uid] = friend_info;
}

void UserMgr::AddFriend(std::shared_ptr<AuthInfo> auth_info)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_info);
    _friend_map[friend_info->_uid] = friend_info;
}

std::shared_ptr<FriendInfo> UserMgr::GetFriendById(int uid)
{
    auto find_it = _friend_map.find(uid);
    if(find_it == _friend_map.end()){
        return nullptr;
    }

    return *find_it;
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::GetApplyList()
{
    return _apply_list;
}

void UserMgr::AppendApplyList(QJsonArray array)
{
    // 遍历 QJsonArray 并输出每个元素
    for (const QJsonValue &value : array)
    {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name,
                           desc, icon, nick, sex, status);
        _apply_list.push_back(info);
        _apply_map.insert(uid, info);
    }
}

void UserMgr::AddApplyList(std::shared_ptr<ApplyInfo> app)
{
    _apply_map.insert(app->_uid, app);
    _apply_list.push_back(app);
}

bool UserMgr::AlreadyApply(int uid)
{
//    for(auto& apply: _apply_list){
//        if(apply->_uid == uid){
//            return true;
//        }
//    }

//    return false;
    return _apply_map.find(uid) != _apply_map.end();
}

std::shared_ptr<ApplyInfo> UserMgr::GetApplyInfoByUid(int uid)
{
    if (_apply_map.find(uid) != _apply_map.end())
    {
        return _apply_map.value(uid);
    }

    return nullptr;
}

std::vector<std::shared_ptr<FriendInfo>> UserMgr::GetConListPerPage() {
    if (_friend_list.empty() || _chat_count_per_page <= 0)
    {
        return {};
    }

    size_t begin = _contact_loaded;
    int end = std::min(begin + _chat_count_per_page, _friend_list.size());

    if (begin >= _friend_list.size()) {
        return {};
    }

    return std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin() + begin, _friend_list.begin() + end);
}

void UserMgr::UpdateContactLoadedCount()
{
    size_t begin = _contact_loaded;
    size_t end = begin + _chat_count_per_page;

    if (begin >= _friend_list.size()) {
        return;
    }

    if (end > _friend_list.size())
    {
        _contact_loaded = _friend_list.size();
        return;
    }

    _contact_loaded = end;
}

bool UserMgr::IsLoadConFinal()
{
    if (_contact_loaded >= _friend_list.size()) {
        return true;
    }

    return false;
}

std::vector<std::shared_ptr<FriendInfo> > UserMgr::GetChatListPerPage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    size_t begin = _chat_loaded;
    size_t end = begin + _chat_count_per_page;

    if (begin >= _friend_list.size()) {
        return friend_list;
    }

    if (end > _friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin() + begin, _friend_list.end());
        return friend_list;
    }


    friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin() + begin, _friend_list.begin()+ end);
    return friend_list;
}

bool UserMgr::IsLoadChatFinal()
{
    if (_chat_loaded >= _friend_list.size()) {
        return true;
    }

    return false;
}

void UserMgr::UpdateChatLoadedCount()
{
    size_t begin = _chat_loaded;
    size_t end = begin + _chat_count_per_page;

    if (begin >= _friend_list.size()) {
        return ;
    }

    if (end > _friend_list.size()) {
        _chat_loaded = _friend_list.size();
        return ;
    }

    _chat_loaded = end;
}

void UserMgr::AppendFriendChatMsg(int friend_id, std::vector<std::shared_ptr<TextChatData>> msgs)
{
    auto find_iter = _friend_map.find(friend_id);
    if(find_iter == _friend_map.end()){
        qDebug()<<"append friend uid  " << friend_id << " not found";
        return;
    }

    find_iter.value()->AppendChatMsgs(msgs);
}

void UserMgr::AppendSlefChatMsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{

    QString last_msg = "";
    for(const auto & text: msgs)
    {
      last_msg = text->_msg_content;
      _user_info->_last_msg = last_msg;
      _user_info->_chat_msgs.push_back(text);
    }

    int fromid = 0, toid = 0;
    if (!msgs.empty())
    {
        fromid = msgs.at(0)->_from_uid; // msgs 中只有一个数据
        toid = msgs.at(0)->_to_uid; // msgs 中只有一个数据
    }

    for (const auto& friend_info : _friend_list)
    {
        if (fromid == friend_info->_uid || friend_info->_uid == toid)
            friend_info->_last_msg = last_msg;
    }
}
