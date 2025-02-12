#ifndef MESSAGE_COMMANDS_H
#define MESSAGE_COMMANDS_H

#include <QString>

namespace MessageCommand
{
    // 界面交互相关命令
    const QString MULTI_SELECT_REQ = "MULTI_SELECT_REQ"; // 消息选择点击多选发出
    const QString DELETE_MULTI_SELECT_REQ = "DELETE_MULTI_SELECT_REQ"; // 删除多选的消息
    const QString DELETE_SINGLE_SELECT_REQ = "DELETE_SINGLE_SELECT_REQ"; // 删除多选的消息
    const QString SEARCH_EDIT_TEXT_CHANGED = "SEARCH_EDIT_TEXT_CHANGED"; // 搜索框文本改变

    // 用户相关命令
    const QString LOGIN = "Login";
    const QString LOGOUT = "Logout";
    const QString UPDATE_USER_INFO = "UpdateUserInfo";

    // 消息相关命令
    const QString SEND_MESSAGE = "SendMessage";
    const QString RECEIVE_MESSAGE = "ReceiveMessage";

    // 系统相关命令
    const QString NETWORK_STATUS = "NetworkStatus";
    const QString ERROR = "Error";
}

#endif // MESSAGE_COMMANDS_H
