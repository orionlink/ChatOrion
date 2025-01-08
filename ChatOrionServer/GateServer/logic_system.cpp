#include "logic_system.h"
#include "http_connection.h"
#include "MySQLManager.h"
#include "RedisManager.h"
#include "verify_grpc_client.h"

LogicSystem::LogicSystem()
{
    regGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->_response.body()) << "receive get_test req"<< std::endl;
        int i = 0;
        for (auto& elem : connection->_get_params) {
            i++;
            beast::ostream(connection->_response.body()) << "param" << i << " key is " << elem.first;
            beast::ostream(connection->_response.body()) << ", " <<  " value is " << elem.second << std::endl;
        }
    });

    regPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
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
    });

    regPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
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
    });
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
