#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include <QString>

extern std::function<void(QWidget*)> repolish;

enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1001, //Json解析失败
    ERR_NETWORK = 1002,
};

enum Modules{
    REGISTER_MOD = 0, // 注册模块
};

extern QString gate_url_prefix;

Q_DECLARE_METATYPE(ReqId)
Q_DECLARE_METATYPE(ErrorCodes)
Q_DECLARE_METATYPE(Modules)

#endif // GLOBAL_H
