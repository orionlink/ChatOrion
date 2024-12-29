#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>

extern std::function<void(QWidget*)> repolish;

enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,
};

enum Modules{
    REGISTER_MOD = 0, // 注册模块
};

#endif // GLOBAL_H
