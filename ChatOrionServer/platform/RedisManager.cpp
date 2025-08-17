//
// Created by hwk on 2025/1/6.
//

#include "RedisManager.h"
#include "Settings.h"
#include "ConnectionPoolProxy.h"
#include "const.h"
#include "log.h"

#include <hiredis/hiredis.h>
#include <cstring>
#include <iostream>

RedisManager::RedisManager()
{
    auto& settings = config::Settings::GetInstance();
    std::string host =  settings.value("redis/host").toString();
    int port =  settings.value("redis/port").toInt();
    std::string password =  settings.value("redis/password").toString();

    _pool.reset(new RedisConnectPool(5, host.c_str(), port, password.c_str()));
}

RedisManager::~RedisManager()
{
    close();
}

bool RedisManager::set(const std::string& key, const std::string& value)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    // ConnectionPoolProxy<redisContext*, RedisConnectPool> proxy(*_pool.get(),
    //     [this]() {
    //         return this->_pool->getConnection();
    // }, [this](redisContext* conn) {
    //     this->_pool->returnConnection(conn);
    // });

    auto reply = (redisReply*)redisCommand(connect, "SET %s %s", key.c_str(), value.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    //如果返回NULL则说明执行失败
    if (NULL == reply)
    {
        std::cout << "Execut command [ SET " << key << "  "<< value << " ] failure ! " << std::endl;
        return false;
    }
    //如果执行失败则释放连接
    if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0)))
    {
        std::cout << "Execut command [ SET " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }

    std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::get(const std::string& key, std::string& value)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    // ConnectionPoolProxy<redisContext*, RedisConnectPool> proxy(*_pool.get(),
    //     [this]() {
    //         return this->_pool->getConnection();
    // }, [this](redisContext* conn) {
    //     this->_pool->returnConnection(conn);
    // });

    auto reply = (redisReply*)redisCommand(connect, "GET %s", key.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == NULL) {
        std::cout << "[ GET  " << key << " ] failed" << std::endl;
        return false;
    }
    if (reply->type != REDIS_REPLY_STRING)
    {
        std::cout << "[ GET  " << key << " ] failed" << std::endl;
        return false;
    }

    value = reply->str;
    std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
    return true;
}

bool RedisManager::rpush(const std::string& key, const std::string& value)
{
    // ConnectionPoolProxy<redisContext*, RedisConnectPool> proxy(*_pool.get(),
    //     [this]() {
    //         return this->_pool->getConnection();
    // }, [this](redisContext* conn) {
    //     this->_pool->returnConnection(conn);
    // });

    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    auto reply = (redisReply*)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (NULL == reply)
    {
        std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }

    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
    {
        std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }
    std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::rpop(const std::string& key, std::string& value)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    // ConnectionPoolProxy<redisContext*, RedisConnectPool> proxy(*_pool.get(),
    //     [this]() {
    //         return this->_pool->getConnection();
    // }, [this](redisContext* conn) {
    //     this->_pool->returnConnection(conn);
    // });

    auto reply = (redisReply*)redisCommand(connect, "RPOP %s ", key.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
        return false;
    }

    value = reply->str;
    std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::lpush(const std::string& key, const std::string& value)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    auto reply = (redisReply*)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (NULL == reply)
    {
        std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
    {
        std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
        return false;
    }

    std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::lpop(const std::string& key, std::string& value)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    auto reply = (redisReply*)redisCommand(connect, "LPOP %s ", key.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << "Execut command [ LPOP " << key<<  " ] failure ! " << std::endl;
        return false;
    }

    value = reply->str;
    std::cout << "Execut command [ LPOP " << key <<  " ] success ! " << std::endl;
    return true;
}

bool RedisManager::hset(const std::string& key, const std::string& hkey, const std::string& value)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    auto reply = (redisReply*)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER )
    {
        std::cout << "Execut command [ HSet " << key << "  " << hkey <<"  " << value << " ] failure ! " << std::endl;
        return false;
    }

    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::hset(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    auto reply = (redisReply*)redisCommandArgv(connect, 4, argv, argvlen);

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
        return false;
    }

    std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::hget(const std::string& key, const std::string& hkey, std::string &value)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    const char* argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.length();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.length();
    auto reply = (redisReply*)redisCommandArgv(connect, 3, argv, argvlen);

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << "Execut command [ HGet " << key << " "<< hkey <<"  ] failure ! " << std::endl;
        return "";
    }
    value = reply->str;
    std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::del(const std::string& key)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    auto reply = (redisReply*)redisCommand(connect, "DEL %s", key.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << "Execut command [ Del " << key <<  " ] failure ! " << std::endl;
        return false;
    }
    std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
    return true;
}

bool RedisManager::hdel(const std::string &key, const std::string &field)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) {
        return false;
    }

    redisReply* reply = (redisReply*)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
    if (reply == nullptr) {
        std::cerr << "HDEL command failed" << std::endl;
        return false;
    }

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    bool success = false;
    if (reply->type == REDIS_REPLY_INTEGER) {
        success = reply->integer > 0;
    }

    return success;
}

bool RedisManager::existsKey(const std::string& key)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    auto reply = (redisReply*)redisCommand(connect, "exists %s", key.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
    {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        return false;
    }
    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    return true;
}

bool RedisManager::expire(const std::string& key, int seconds)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    auto reply = (redisReply*)redisCommand(connect, "EXPIRE %s %d",
        key.c_str(), seconds);

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
    {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        return false;
    }

    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    return true;
}

bool RedisManager::execute(const std::string& command)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    try {
        redisReply* reply = (redisReply*)redisCommand(connect, command.c_str());
        if (reply == nullptr)
        {
            LOG_ERROR << "Redis execute failed: " << command;
            return false;
        }

        Defer defer([this, &reply, connect] {
            freeReplyObject(reply);
            this->_pool->returnConnection(connect);
        });

        bool success = (reply->type != REDIS_REPLY_ERROR);
        return success;
    } catch (const std::exception& e)
    {
        LOG_ERROR << "Redis execute exception: " << e.what();
        return false;
    }
}

bool RedisManager::lrange(const std::string& key, std::vector<std::string>& values)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    auto reply = (redisReply*)redisCommand(connect, "LRANGE %s 0 -1",
        key.c_str());

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply == nullptr)
    {
        std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
        return false;
    }

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (size_t i = 0; i < reply->elements; i++) {
            values.push_back(reply->element[i]->str);
        }
    }

    std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
    return true;
}

bool RedisManager::hkeys(const std::string& key, std::vector<std::string>& fields)
{
    auto connect = _pool->getConnection();
    if (connect == nullptr) return false;

    try
    {
        redisReply* reply = (redisReply*)redisCommand(connect, "HKEYS %s", key.c_str());
        if (reply == nullptr) {
            LOG_ERROR << "HKEYS command failed for key: " << key;
            return false;
        }

        Defer defer([this, &reply, connect] {
            freeReplyObject(reply);
            this->_pool->returnConnection(connect);
        });

        if (reply->type == REDIS_REPLY_ARRAY)
        {
            for (size_t i = 0; i < reply->elements; i++)
            {
                fields.push_back(reply->element[i]->str);
            }
        }

        return true;
    } catch (const std::exception& e) {
        LOG_ERROR << "HKEYS failed: " << e.what();
        return false;
    }
}

std::vector<std::string> RedisManager::zrevrange(const std::string& key, int start, int stop)
{
    std::vector<std::string> result;

    auto connect = _pool->getConnection();
    if (connect == nullptr) return result;

    redisReply* reply = (redisReply*)redisCommand(
        connect,
        "ZREVRANGE %s %d %d",
        key.c_str(), start, stop
    );

    if (reply == nullptr) {
        std::cout << "Redis zrevrange error: " << connect->errstr;
        return result;
    }

    Defer defer([this, &reply, connect] {
        freeReplyObject(reply);
        this->_pool->returnConnection(connect);
    });

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (size_t i = 0; i < reply->elements; i++)
        {
            if (reply->element[i]->type == REDIS_REPLY_STRING)
            {
                result.push_back(reply->element[i]->str);
            }
        }
    } else {
        std::cout << "Redis zrevrange unexpected reply type: " << reply->type;
    }

    return result;
}

void RedisManager::close()
{
    _pool->close();
}
