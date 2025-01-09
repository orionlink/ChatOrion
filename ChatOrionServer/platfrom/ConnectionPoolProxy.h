//
// Created by hwk on 2025/1/7.
// 连接池代理，RAII管理连接池释放问题
//

#ifndef CONNECTIONPOOLPROXY_H
#define CONNECTIONPOOLPROXY_H

#include <functional>
#include <iostream>
#include <memory>

template <typename ConnectionType, typename PoolType>
class ConnectionPoolProxy {
public:
    // 构造函数：从连接池获取连接
    ConnectionPoolProxy(PoolType& pool, std::function<ConnectionType()> acquireFunc, std::function<void(ConnectionType)> releaseFunc)
        : _pool(pool), _acquireFunc(acquireFunc), _releaseFunc(releaseFunc), _connection(_acquireFunc()) {
        if (!_connection) {
            throw std::runtime_error("Failed to get connection from pool");
        }
    }

    // 析构函数：将连接返回给连接池
    ~ConnectionPoolProxy() {
        if (_connection) {
            _releaseFunc(std::move(_connection));
        }
    }

    // 禁止拷贝构造和拷贝赋值
    ConnectionPoolProxy(const ConnectionPoolProxy&) = delete;
    ConnectionPoolProxy& operator=(const ConnectionPoolProxy&) = delete;

    // 允许移动构造和移动赋值
    ConnectionPoolProxy(ConnectionPoolProxy&& other) noexcept
        : _pool(other._pool), _acquireFunc(other._acquireFunc), _releaseFunc(other._releaseFunc), _connection(std::move(other._connection)) {
        other._connection = nullptr;
    }

    ConnectionPoolProxy& operator=(ConnectionPoolProxy&& other) noexcept {
        if (this != &other) {
            _pool = other._pool;
            _acquireFunc = other._acquireFunc;
            _releaseFunc = other._releaseFunc;
            _connection = std::move(other._connection);
            other._connection = nullptr;
        }
        return *this;
    }

    // 获取底层连接
    ConnectionType get() const {
        return _connection;
    }

    // 重载 -> 操作符，方便直接使用连接
    auto operator->() const {
        if constexpr (std::is_pointer_v<ConnectionType>) {
            return _connection;
        } else {
            return _connection.get();
        }
    }

private:
    PoolType& _pool;                              // 连接池引用
    std::function<ConnectionType()> _acquireFunc; // 获取连接的函数
    std::function<void(ConnectionType)> _releaseFunc; // 释放连接的函数
    ConnectionType _connection;                   // 持有的连接
};

#endif //CONNECTIONPOOLPROXY_H
