//
// Created by hwk on 2025/1/6.
// redis连接类, 有线程安全问题
//

#ifndef REDISMANAGER_H
#define REDISMANAGER_H

#include "singleton.hpp"
#include "RedisConnectPool.h"

struct redisContext;
struct redisReply;

class RedisManager : public Singleton<RedisManager>
{
    friend class Singleton<RedisManager>;
public:
    ~RedisManager();

    bool set(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value);

    bool rpush(const std::string& key, const std::string& value);
    bool rpop(const std::string& key, std::string& value);

    bool lpush(const std::string& key, const std::string& value);
    bool lpop(const std::string& key, std::string& value);

    bool hset(const std::string &key, const std::string  &hkey, const std::string &value);
    bool hset(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    bool hget(const std::string &key, const std::string &hkey, std::string &value);

    bool del(const std::string &key);
    bool hdel(const std::string& key, const std::string& field);
    bool existsKey(const std::string &key);
    bool expire(const std::string& key, int seconds);
    // 执行原始Redis命令
    bool execute(const std::string& command);
    // 获取列表所有元素
    bool lrange(const std::string& key, std::vector<std::string>& values);

    bool hkeys(const std::string& key, std::vector<std::string>& fields);
    // 倒序获取有序集合的范围数据
    std::vector<std::string> zrevrange(const std::string& key, int start, int stop);
    void close();
private:
    RedisManager();

    std::unique_ptr<RedisConnectPool> _pool;
};

#endif //REDISMANAGER_H
