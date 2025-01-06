//
// Created by hwk on 2025/1/6.
//

#include "RedisConnectPool.h"

#include <iostream>
#include <hiredis/hiredis.h>

RedisConnectPool::RedisConnectPool(size_t pool_size, const char* host, int port, const char* passwd)
    :_pool_size(pool_size), _is_stop(false), _host(host), _port(port)
{
    for (int i = 0; i < _pool_size; ++i)
    {
        auto connect = redisConnect(_host, _port);
        if (connect == nullptr || connect->err)
        {
            if (connect != nullptr)
            {
                std::cerr << "redis " << i << "连接失败: "<< connect->errstr << std::endl;
                redisFree(connect);
                connect = nullptr;
            }
            continue;
        }

        auto reply = (redisReply*)redisCommand(connect, "AUTH %s", passwd);
        if (reply->type == REDIS_REPLY_ERROR)
        {
            std::cout << "redis " << i << "认证失败" << std::endl;
            freeReplyObject(reply);
            continue;
        }

        std::cout << "redis " << i << "认证成功" << std::endl;
        freeReplyObject(reply);
        _connections.push(connect);
    }
}

RedisConnectPool::~RedisConnectPool()
{
    std::lock_guard<std::mutex> lock(_mutex);
    while (!_connections.empty())
    {
        auto connect = _connections.front();
        _connections.pop();
        redisFree(connect);
    }
}

redisContext* RedisConnectPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condition_var.wait(lock, [this]
    {
        if (_is_stop)
            return true;

        return !_connections.empty();
    });

    //如果停止则直接返回空指针
    if (_is_stop)
    {
        return nullptr;
    }

    auto context = _connections.front();
    _connections.pop();
    return context;
}

void RedisConnectPool::returnConnection(redisContext* context)
{
    if (context == nullptr) return;

    std::lock_guard<std::mutex> lock(_mutex);

    if (_is_stop) return;

    _connections.push(context);
    _condition_var.notify_one();
}

void RedisConnectPool::close()
{
    _is_stop = true;
    _condition_var.notify_all();
}
