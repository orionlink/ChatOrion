#ifndef USERINFO_H
#define USERINFO_H

#include <QString>

struct UserInfo
{
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = ""):
        _uid(uid),_name(name),_nick(nick),_icon(icon),_sex(sex),_last_msg(last_msg){}

    UserInfo(int uid, QString name, QString icon):
    _uid(uid), _name(name), _icon(icon),_nick(_name),
    _sex(0),_last_msg(""){

    }

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _last_msg;
};

#endif // USERINFO_H
