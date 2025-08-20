//
// Created by hwk on 25-8-20.
//

#ifndef ORION_MESSAGE_MESSAGE_TYPE_H
#define ORION_MESSAGE_MESSAGE_TYPE_H

namespace message
{
    enum MSG_IDS
    {
        ID_GET_VARIFY_CODE = 1001,                      //获取验证码
        ID_REG_USER = 1002,                             //注册用户
        ID_RESET_PWD = 1003,                            //忘记密码
        ID_LOGIN_USER = 1004,                           // 用户密码登录
        ID_LOGIN_EMAIL = 1005,                          // 邮箱登录
        MSG_CHAT_LOGIN = 1006,                          //用户登陆(聊天服务器登录)
        MSG_CHAT_LOGIN_RSP = 1007,                      //用户登陆回包(聊天服务器登录回包)
        ID_SEARCH_USER_REQ = 1008,                      //用户搜索请求
        ID_SEARCH_USER_RSP = 1009,                      //搜索用户回包
        ID_ADD_FRIEND_REQ = 1010,                       //申请添加好友请求
        ID_ADD_FRIEND_RSP  = 1011,                      //申请添加好友回复
        ID_NOTIFY_ADD_FRIEND_REQ = 1012,                //通知用户添加好友申请
        ID_AUTH_FRIEND_REQ = 1013,                      //认证好友请求
        ID_AUTH_FRIEND_RSP = 1014,                      //认证好友回复
        ID_NOTIFY_AUTH_FRIEND_REQ = 1015,               //通知用户认证好友申请
        ID_TEXT_CHAT_MSG_REQ = 1017,                    //文本聊天信息请求
        ID_TEXT_CHAT_MSG_RSP = 1018,                    //文本聊天信息回复
        ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,             //通知用户文本聊天信息
        ID_GET_HISTORY_MSG_REQ = 1020,                  // 获取历史消息请求
        ID_GET_HISTORY_MSG_RSP = 1021,                  // 获取历史消息响应
        ID_MARK_MSG_READ_REQ = 1022,                    // 标记已读请求
        ID_MARK_MSG_READ_RSP = 1023,                    // 标记已读响应
        ID_LOGIN_LOAD_UNREAD_CHAT_MSG = 1024,           // 登录加载未读聊天信息
    };

    enum ErrorCodes {
        Success = 0,
        Error_Json = 1001,                  //Json解析错误
        RPCFailed = 1002,                   //RPC请求错误
        VarifyExpired = 1003,               // 验证码过期
        VarifyCodeErr = 1004,               // 验证码错误
        UserExist = 1005,                   // 用户存在
        PasswdErr = 1006,                   // 密码错误
        EmailNotMatch = 1007,               // 邮箱不匹配
        PasswdUpdataFailed = 1008,          // 更新密码失败
        PasswdInvalid = 1009,               // 密码无效
        UserNotExist = 1010,                // 用户不存在
        TokenInvalid = 1011,                //Token失效
        UidInvalid = 1012,                  //uid无效
        DbError = 1013,                     // 数据库错误
    };

    enum MessageType
    {
        TEXT = 1,                           //文本消息
    };
}

#endif //ORION_MESSAGE_MESSAGE_TYPE_H
