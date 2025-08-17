//
// Created by hwk on 2025/1/7.
//

#ifndef MYSQLCONNECTPOOL_H
#define MYSQLCONNECTPOOL_H

#include <thread>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

#include "const.h"

struct SqlConnection
{
    SqlConnection(sql::Connection* conn, int64_t last_time)
        :_connection(conn), _last_oper_time(last_time) {}
    std::unique_ptr<sql::Connection> _connection;
    int64_t _last_oper_time;
};

class MySQLConnectPool
{
public:
    MySQLConnectPool(size_t pool_size, const std::string& url, const std::string& user,
        const std::string& password, const std::string& database);
    ~MySQLConnectPool();

    std::unique_ptr<SqlConnection> getConnection();

    void returnConnection(std::unique_ptr<SqlConnection> conn);

    void close();
private:
    void checkConnection();

    std::string _url;
    std::string _user;
    std::string _password;
    std::string _database;

    std::queue<std::unique_ptr<SqlConnection>> _pool;
    size_t _pool_size;
    std::atomic<bool> _is_stop;
    std::mutex _mutex;
    std::condition_variable _condition_var;
    std::thread _check_thread;
};

#endif //MYSQLCONNECTPOOL_H
