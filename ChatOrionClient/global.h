#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include <QString>

extern std::function<void(QWidget*)> repolish;

enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002,         //注册用户
    ID_RESET_PWD = 1003,        //忘记密码
    ID_LOGIN_USER = 1004,       // 用户密码登录
    ID_LOGIN_EMAIL = 1005,       // 邮箱登录
    ID_CHAT_LOGIN = 1006, //登陆聊天服务器

    ID_CHAT_LOGIN_RSP = 1007, //登陆聊天服务器回包
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1001,        //Json解析失败
    ERR_NETWORK = 1002,
    VarifyExpired = 1003,   // 验证码过期
    VarifyCodeErr = 1004,   // 验证码错误
    UserExist = 1005,       // 用户存在
    PasswdErr = 1006,       // 密码错误
    EmailNotMatch = 1007,   // 邮箱不匹配
    PasswdUpdataFailed = 1008,// 更新密码失败
    PasswdInvalid = 1009    // 密码无效
};

enum Modules{
    REGISTER_MOD = 0, // 注册模块
    LOGIN_MOD = 1, // 登录模块（邮箱登录和用户密码登录）
};

extern QString gate_url_prefix;

Q_DECLARE_METATYPE(ReqId)
Q_DECLARE_METATYPE(ErrorCodes)
Q_DECLARE_METATYPE(Modules)


#endif // GLOBAL_H
