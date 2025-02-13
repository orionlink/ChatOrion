#include "user_mgr.h"

#include <QJsonArray>

UserMgr::UserMgr():_user_info(nullptr), _contact_loaded(0), _chat_count_per_page(13)
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
