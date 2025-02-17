//
// Created by hwk on 2025/1/7.
//

#include "MySQLConnectPool.h"

MySQLConnectPool::MySQLConnectPool(size_t pool_size, const std::string& url, const std::string& user,
    const std::string& password, const std::string& database)
    :_pool_size(pool_size), _url(url), _user(user), _password(password),
    _database(database), _is_stop(false)
{
    try
    {
        for (int i = 0; i < _pool_size; ++i)
        {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto conn = driver->connect(_url, _user, _password);
            conn->setSchema(_database);

            auto currentTime = std::chrono::system_clock::now().time_since_epoch();
            // 将时间戳转换为秒
            long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
            _pool.push(std::make_unique<SqlConnection>(conn, timestamp));

            std::cout << "mysql connect (" << i + 1<< ") success" << std::endl;

            _check_thread = std::thread([this]
            {
                while (!_is_stop)
                {
                    checkConnection();
                    std::this_thread::sleep_for(std::chrono::seconds(60));
                }
            });
            _check_thread.detach();
        }
    }
    catch (sql::SQLException& e)
    {
        std::cout << "mysql pool init failed, error is " << e.what()<< std::endl;
    }
}

MySQLConnectPool::~MySQLConnectPool()
{
    std::unique_lock<std::mutex> lock(_mutex);
    while (!_pool.empty()) _pool.pop();
}

std::unique_ptr<SqlConnection> MySQLConnectPool::getConnection()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _condition_var.wait(lock, [this]
    {
        if (_is_stop) return true;

        return !_pool.empty();
    });

    if (_is_stop) return nullptr;

    std::unique_ptr<SqlConnection> conn = std::move(_pool.front());
    _pool.pop();
    return conn;
}

void MySQLConnectPool::returnConnection(std::unique_ptr<SqlConnection> conn)
{
    std::unique_lock<std::mutex> lock(_mutex);

    if (_is_stop) return;

    _pool.push(std::move(conn));
    _condition_var.notify_one();
}

void MySQLConnectPool::close()
{
    _is_stop = true;
    _condition_var.notify_all();
}

void MySQLConnectPool::checkConnection()
{
    std::lock_guard<std::mutex> guard(_mutex);
    int pool_size = _pool.size();

    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
    for (int i = 0; i < pool_size; i++)
    {
        auto conn = std::move(_pool.front());
        _pool.pop();

        Defer defer([this, &conn]()
        {
            _pool.push(std::move(conn));
        });

        if (timestamp - conn->_last_oper_time < 600)
            continue;

        try
        {
            std::unique_ptr<sql::Statement> stmt(conn->_connection->createStatement());
            stmt->executeQuery("SELECT 1");
            conn->_last_oper_time = timestamp;
        }
        catch (sql::SQLException& e)
        {
            std::cout << "Error keeping connection alive: " << e.what() << std::endl;
            // 重新创建连接并替换旧的连接
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto* newcon = driver->connect(_url, _user, _password);
            newcon->setSchema(_database);
            conn->_connection.reset(newcon);
            conn->_last_oper_time = timestamp;
        }
    }
}
