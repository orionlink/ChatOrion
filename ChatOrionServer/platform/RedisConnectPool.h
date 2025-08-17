//
// Created by hwk on 2025/1/6.
// redis 连接池
//

#ifndef REDISCONNECTPOOL_H
#define REDISCONNECTPOOL_H

#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

struct redisContext;

class RedisConnectPool
{
public:
    RedisConnectPool(size_t pool_size, const char* host, int port, const char* passwd);
    ~RedisConnectPool();

    redisContext* getConnection();

    void returnConnection(redisContext*);

    void close();
private:
    std::queue<redisContext*> _connections;
    std::atomic<bool> _is_stop;
    std::mutex _mutex;
    std::condition_variable _condition_var;

    size_t _pool_size;
    const char* _host;
    int _port;
};

#endif //REDISCONNECTPOOL_H
