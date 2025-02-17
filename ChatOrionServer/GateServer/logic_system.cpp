#include "logic_system.h"
#include "http_connection.h"
#include "MySQLManager.h"
#include "RedisManager.h"
#include "verify_grpc_client.h"
#include "StatusGrpcClient.h"

LogicSystem::LogicSystem()
{
    regPost("/get_varifycode", std::bind(&LogicSystem::getVarifycodeCallback, this, std::placeholders::_1));

    regPost("/user_register", std::bind(&LogicSystem::userRegsterCallback, this, std::placeholders::_1));

    regPost("/reset_pwd", std::bind(&LogicSystem::forgetPasswordCallback, this, std::placeholders::_1));

    regPost("/user_login", std::bind(&LogicSystem::userLoginCallback, this, std::placeholders::_1));

    regPost("/email_login", std::bind(&LogicSystem::emailLoginCallback, this, std::placeholders::_1));
}

void LogicSystem::getVarifycodeCallback(std::shared_ptr<HttpConnection> connection)
{
    auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
    // std::cout << "receive body is " << body_str << std::endl;
    connection->_response.set(http::field::content_type, "text/json");

    Json::Value root;
    Json::Value src_root;
    Json::Reader reader;
    bool sueccss = reader.parse(body_str, src_root);
    if (!sueccss)
    {
        std::cout << "Failed to parse JSON data!" << std::endl;
        root["error"] = ErrorCodes::Error_Json;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    std::string email = src_root["email"].asString();
    message::GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);
    std::cout << "email is " << email << std::endl;
    root["error"] = rsp.error();
    root["email"] = src_root["email"];
    std::string jsonstr = root.toStyledString();
    boost::beast::ostream(connection->_response.body()) << jsonstr;
}

void LogicSystem::userRegsterCallback(std::shared_ptr<HttpConnection> connection)
{
    auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
    // std::cout << "receive body is " << body_str << std::endl;
    connection->_response.set(http::field::content_type, "text/json");

    Json::Value root;
    Json::Value src_root;
    Json::Reader reader;
    bool sueccss = reader.parse(body_str, src_root);
    if (!sueccss)
    {
        std::cout << "Failed to parse JSON data!" << std::endl;
        root["error"] = ErrorCodes::Error_Json;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    std::string username = src_root["username"].asString();
    std::string password = src_root["password"].asString();
    std::string email = src_root["email"].asString();
    std::string code = src_root["code"].asString();

    //先查找redis中email对应的验证码是否合理
    std::string varify_code;
    bool b_get_varify = RedisManager::GetInstance()->get(CODE_PREFIX + email, varify_code);
    if (!b_get_varify)
    {
        std::cout << "get varify code expired" << std::endl;
        root["error"] = ErrorCodes::VarifyExpired;
        root["error_msg"] = "验证码不存在";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    if (varify_code != code)
    {
        std::cout << "varify code error" << std::endl;
        root["error"] = ErrorCodes::VarifyCodeErr;
        root["error_msg"] = "验证码错误";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    //查找数据库判断用户是否存在
    int uid = MySQLManager::GetInstance()->registerUser(username, email, password, "");
    if (uid == 0 || uid == -1)
    {
        std::cout << " user or email exist" << std::endl;
        root["error"] = ErrorCodes::UserExist;
        root["error_msg"] = "用户或邮箱已经存在";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    root["error"] = 0;
    root["email"] = email;
    root ["username"]= username;
    root["password"] = password;
    root["code"] = code;
    std::string jsonstr = root.toStyledString();
    beast::ostream(connection->_response.body()) << jsonstr;
}

void LogicSystem::forgetPasswordCallback(std::shared_ptr<HttpConnection> connection)
{
    auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
    // std::cout << "receive body is " << body_str << std::endl;
    connection->_response.set(http::field::content_type, "text/json");

    Json::Value root;
    Json::Value src_root;
    Json::Reader reader;
    bool sueccss = reader.parse(body_str, src_root);
    if (!sueccss)
    {
        std::cout << "Failed to parse JSON data!" << std::endl;
        root["error"] = ErrorCodes::Error_Json;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    std::string username = src_root["username"].asString();
    std::string password = src_root["password"].asString();
    std::string email = src_root["email"].asString();
    std::string code = src_root["code"].asString();

    //先查找redis中email对应的验证码是否合理
    std::string varify_code;
    bool b_get_varify = RedisManager::GetInstance()->get(CODE_PREFIX + email, varify_code);
    if (!b_get_varify)
    {
        std::cout << "get varify code expired" << std::endl;
        root["error"] = ErrorCodes::VarifyExpired;
        root["error_msg"] = "验证码不存在";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    if (varify_code != code)
    {
        std::cout << "varify code error" << std::endl;
        root["error"] = ErrorCodes::VarifyCodeErr;
        root["error_msg"] = "验证码错误";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    //查询数据库判断用户名和邮箱是否匹配
    bool email_valid = MySQLManager::GetInstance()->checkUsernameEmailMatch(username, email);
    if (!email_valid) {
        std::cout << " user email not match" << std::endl;
        root["error"] = ErrorCodes::EmailNotMatch;
        root["error_msg"] = "邮箱或用户名不匹配";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    //更新密码为最新密码
    bool b_up = MySQLManager::GetInstance()->updatePassword(username, password);
    if (!b_up) {
        std::cout << " update pwd failed" << std::endl;
        root["error"] = ErrorCodes::PasswdUpdataFailed;
        root["error_msg"] = "密码更新失败，请重试";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    std::cout << "succeed to update password" << password << std::endl;
    root["error"] = 0;
    root["email"] = email;
    root["username"] = username;
    root["password"] = password;
    root["code"] = src_root["code"].asString();
    std::string jsonstr = root.toStyledString();
    beast::ostream(connection->_response.body()) << jsonstr;
    return;
}

void LogicSystem::userLoginCallback(std::shared_ptr<HttpConnection> connection)
{
    auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
    // std::cout << "receive body is " << body_str << std::endl;
    connection->_response.set(http::field::content_type, "text/json");

    Json::Value root;
    Json::Value src_root;
    Json::Reader reader;
    bool sueccss = reader.parse(body_str, src_root);
    if (!sueccss)
    {
        std::cout << "Failed to parse JSON data!" << std::endl;
        root["error"] = ErrorCodes::Error_Json;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    std::string username = src_root["username"].asString();
    std::string password = src_root["password"].asString();

    // 查询数据库
    UserInfo user_info;
    int result = MySQLManager::GetInstance()->checkUserLogin(username, password, user_info);
    if (result)
    {
        std::cout << "checkUserLogin result: "  << result << std::endl;
        if (result == 1)
        {
            root["error"] = ErrorCodes::UserNotExist;
            root["error_msg"] = "用户名不存在，请注册";
        }
        else
        {
            root["error"] = ErrorCodes::PasswdInvalid;
            root["error_msg"] = "用户名或密码错误";
        }
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    //查询StatusServer找到合适的连接
    auto reply = StatusGrpcClient::GetInstance()->GetChatServer(user_info.uid);
    if (reply.error())
        {
        std::cout << " grpc get chat server failed, error is " << reply.error()<< std::endl;
        root["error"] = ErrorCodes::RPCFailed;
        root["error_msg"] = "服务器出错";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    std::cout << "succeed to load userinfo uid is " << user_info.uid << std::endl;
    root["error"] = 0;
    root["username"] = username;
    root["uid"] = user_info.uid;
    root["token"] = reply.token();
    root["host"] = reply.host();
    root["port"] = reply.port();
    std::string jsonstr = root.toStyledString();
    beast::ostream(connection->_response.body()) << jsonstr;
}

void LogicSystem::emailLoginCallback(std::shared_ptr<HttpConnection> connection)
{
    auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
    // std::cout << "receive body is " << body_str << std::endl;
    connection->_response.set(http::field::content_type, "text/json");

    Json::Value root;
    Json::Value src_root;
    Json::Reader reader;
    bool sueccss = reader.parse(body_str, src_root);
    if (!sueccss)
    {
        std::cout << "Failed to parse JSON data!" << std::endl;
        root["error"] = ErrorCodes::Error_Json;
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    std::string email = src_root["email"].asString();
    std::string code = src_root["code"].asString();

    //先查找redis中email对应的验证码是否合理
    std::string varify_code;
    bool b_get_varify = RedisManager::GetInstance()->get(CODE_PREFIX + email, varify_code);
    if (!b_get_varify)
    {
        std::cout << "get varify code expired" << std::endl;
        root["error"] = ErrorCodes::VarifyExpired;
        root["error_msg"] = "验证码不存在";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    if (varify_code != code)
    {
        std::cout << "varify code error" << std::endl;
        root["error"] = ErrorCodes::VarifyCodeErr;
        root["error_msg"] = "验证码错误";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    // 查询数据库
    UserInfo user_info;
    bool success = MySQLManager::GetInstance()->checkEmailLoginWithCode(email, user_info);
    if (!success)
    {
        std::cout << " user pwd not match" << std::endl;
        root["error"] = ErrorCodes::EmailNotMatch;
        root["error_msg"] = "登录失败，该邮箱尚未注册";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    //查询StatusServer找到合适的连接
    auto reply = StatusGrpcClient::GetInstance()->GetChatServer(user_info.uid);
    if (reply.error())
    {
        std::cout << " grpc get chat server failed, error is " << reply.error()<< std::endl;
        root["error"] = ErrorCodes::RPCFailed;
        root["error_msg"] = "服务器出错";
        std::string jsonstr = root.toStyledString();
        beast::ostream(connection->_response.body()) << jsonstr;
        return;
    }

    std::cout << "succeed to load userinfo uid is " << user_info.uid << std::endl;
    root["error"] = 0;
    root["username"] = user_info.name;
    root["uid"] = user_info.uid;
    root["token"] = reply.token();
    root["host"] = reply.host();
    root["port"] = reply.port();
    std::string jsonstr = root.toStyledString();
    beast::ostream(connection->_response.body()) << jsonstr;
}

bool LogicSystem::handleGet(const std::string& path, std::shared_ptr<HttpConnection> con)
{
    if (_get_handlers.find(path) == _get_handlers.end())
        return false;
    _get_handlers[path](con);
    return true;
}

bool LogicSystem::handlePost(const std::string& path, std::shared_ptr<HttpConnection> con)
{
    if (_post_handlers.find(path) == _post_handlers.end())
        return false;
    _post_handlers[path](con);
    return true;
}

void LogicSystem::regGet(std::string url, HttpHandler handler)
{
    auto iter = _get_handlers.find(url);
    if (iter == _get_handlers.end())
    {
        _get_handlers[url] = handler;
    }
}

void LogicSystem::regPost(std::string url, HttpHandler handler)
{
    auto iter = _post_handlers.find(url);
    if (iter == _post_handlers.end())
    {
        _post_handlers[url] = handler;
    }
}
