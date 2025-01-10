#ifndef CONST_H
#define CONST_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>

#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include <iostream>
#include <functional>
#include <string>
#include <memory>
#include <queue>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

enum ErrorCodes {
    Success = 0,
    Error_Json = 1001,      //Json解析错误
    RPCFailed = 1002,       //RPC请求错误
    VarifyExpired = 1003,   // 验证码过期
    VarifyCodeErr = 1004,   // 验证码错误
    UserExist = 1005,       // 用户存在
    PasswdErr = 1006,       // 密码错误
    EmailNotMatch = 1007,   // 邮箱不匹配
    PasswdUpdataFailed = 1008,// 更新密码失败
    PasswdInvalid = 1009,    // 密码无效
    UserNotExist = 1010    // 用户不存在
};

const std::string CODE_PREFIX = "code_";

class Defer
{
public:
    Defer(std::function<void()> func) : _func(func) {}
    ~Defer()
    {
        _func();
    }
private:
    std::function<void()> _func;
};

struct UserInfo {
    std::string name;
    std::string pwd;
    int uid;
    std::string email;
};

const std::string LOGIN_COUNT = "logincount";
const std::string USERTOKENPREFIX = "utoken_";

#endif // CONST_H
