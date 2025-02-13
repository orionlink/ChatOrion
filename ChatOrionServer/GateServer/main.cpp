#include <iostream>

#include "const.h"
#include "cserver.h"
#include "Settings.h"
#include "log.h"

#include <hiredis/hiredis.h>
#include <mysqlx/xdevapi.h>

#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/exception.h>

#include "MySQLManager.h"
#include "RedisManager.h"

void TestRedis();
void TestRedisManager();
void test_mysql_connection();

int main(int argc, char *argv[])
{
    // TestRedis();
    // test_mysql_connection();

    try
    {
        auto &settings = config::Settings::GetInstance();
        settings.setFileName("config.ini");
        settings.load();

        // TestRedisManager();
        auto log = Log::GetInstance();
        if (!log->Initialize(argv[0], true))
        {
            std::cerr << "日志模块初始化失败" << std::endl;
            return -1;
        }

        if (!MySQLManager::GetInstance()->init())
        {
            LOG_ERROR << "数据库初始化失败";
            return -2;
        }

        unsigned short port = settings.value("GateServer/port", 8080).toInt();
        net::io_context ioc{1};
        boost::asio::signal_set signal(ioc, SIGINT, SIGTERM);
        signal.async_wait([&ioc](boost::system::error_code err, int signal_number)
        {
            if (err) return;

            ioc.stop();
        });
        std::make_shared<CServer>(ioc, port)->start();
        LOG_INFO << "Gate Server listen on port: " << port;
        std::cout << "主线程 id: " << std::this_thread::get_id() << std::endl;

        ioc.run();
    }
    catch (std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}

void TestRedisManager()
{
    assert(RedisManager::GetInstance()->set("blogwebsite","llfc.club"));
    std::string value="";
    assert(RedisManager::GetInstance()->get("blogwebsite", value) );
    assert(RedisManager::GetInstance()->get("nonekey", value) == false);
    assert(RedisManager::GetInstance()->hset("bloginfo","blogwebsite", "llfc.club"));
    assert(RedisManager::GetInstance()->hget("bloginfo","blogwebsite", value));
    assert(RedisManager::GetInstance()->existsKey("bloginfo"));
    assert(RedisManager::GetInstance()->del("bloginfo"));
    assert(RedisManager::GetInstance()->del("bloginfo"));
    assert(RedisManager::GetInstance()->existsKey("bloginfo") == false);
    assert(RedisManager::GetInstance()->lpush("lpushkey1", "lpushvalue1"));
    assert(RedisManager::GetInstance()->lpush("lpushkey1", "lpushvalue2"));
    assert(RedisManager::GetInstance()->lpush("lpushkey1", "lpushvalue3"));
    assert(RedisManager::GetInstance()->rpop("lpushkey1", value));
    assert(RedisManager::GetInstance()->rpop("lpushkey1", value));
    assert(RedisManager::GetInstance()->lpop("lpushkey1", value));
    assert(RedisManager::GetInstance()->lpop("lpushkey2", value)==false);
}

void TestRedis()
{
    redisContext* c = redisConnect("localhost", 6379);
    if (c->err)
    {
        printf("Connect to redisServer faile:%s\n", c->errstr);
        redisFree(c);
        return;
    }
    printf("Connect to redisServer Success\n");
    std::string redis_password = "123456";
    redisReply* r = (redisReply*)redisCommand(c, "AUTH %s %s", "default", redis_password.c_str());
     if (r->type == REDIS_REPLY_ERROR) {
         printf("Redis认证失败！错误信息：%s\n", r->str);
    }
    else
    {
        printf("Redis认证成功！\n");
    }
    //为redis设置key
    const char* command1 = "set stest1 value1";
    //执行redis命令行
    r = (redisReply*)redisCommand(c, command1);
    //如果返回NULL则说明执行失败
    if (NULL == r)
    {
        printf("Execut command1 failure\n");
        redisFree(c);
        return;
    }
    //如果执行失败则释放连接
    if (!(r->type == REDIS_REPLY_STATUS && (strcmp(r->str, "OK") == 0 || strcmp(r->str, "ok") == 0)))
    {
        printf("Failed to execute command[%s]\n", command1);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
    freeReplyObject(r);
    printf("Succeed to execute command[%s]\n", command1);
    const char* command2 = "strlen stest1";
    r = (redisReply*)redisCommand(c, command2);
    //如果返回类型不是整形 则释放连接
    if (r->type != REDIS_REPLY_INTEGER)
    {
        printf("Failed to execute command[%s]\n", command2);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    //获取字符串长度
    int length = r->integer;
    freeReplyObject(r);
    printf("The length of 'stest1' is %d.\n", length);
    printf("Succeed to execute command[%s]\n", command2);
    //获取redis键值对信息
    const char* command3 = "get stest1";
    r = (redisReply*)redisCommand(c, command3);
    if (r->type != REDIS_REPLY_STRING)
    {
        printf("Failed to execute command[%s]\n", command3);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    printf("The value of 'stest1' is %s\n", r->str);
    freeReplyObject(r);
    printf("Succeed to execute command[%s]\n", command3);
    const char* command4 = "get stest2";
    r = (redisReply*)redisCommand(c, command4);
    if (r->type != REDIS_REPLY_NIL)
    {
        printf("Failed to execute command[%s]\n", command4);
        freeReplyObject(r);
        redisFree(c);
        return;
    }
    freeReplyObject(r);
    printf("Succeed to execute command[%s]\n", command4);
    //释放连接资源
    redisFree(c);
}

// 测试函数：验证 MySQL Connector/C++ 是否链接成功
void test_mysql_connection()
{
    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        std::unique_ptr<sql::Connection> con(driver->connect("127.0.0.1", "root", "123456"));
    }
    catch (sql::SQLException& e) {
        // 处理异常
        std::cout << "mysql pool init failed: " << e.what() << std::endl;
    }
}