//
// Created by hwk on 2025/1/7.
//

#include "MySQLDao.h"
#include "Settings.h"
#include "tools.h"

MySQLDao::MySQLDao()
{
    config::Settings& settings = config::Settings::GetInstance();
    std::string host = settings.value<std::string>("MySQL/host", "127.0.0.1").toString();
    int port = settings.value("MySQL/port", 3306).toInt();
    std::string url = host + ":" + std::to_string(port);
    std::string user = settings.value("MySQL/user", "root").toString();
    std::string password = settings.value("MySQL/password", "123456").toString();
    std::string database = settings.value("MySQL/database", "ChatOrion").toString();
    _pool.reset(new MySQLConnectPool(5, url, user, password, database));

    std::string sql_content = Tools::ReadFile("user.sql");
    if (!sql_content.empty())
    {
        try
        {
            auto conn = _pool->getConnection();
            Defer defer([&conn, this] {
                if (conn != nullptr) _pool->returnConnection(std::move(conn));
            });

            std::unique_ptr<sql::Statement> stmt(conn->_connection->createStatement());

            // 拆分 SQL 脚本为多个语句
            std::vector<std::string> sql_statements = splitSQLScript(sql_content);

            // 逐条执行 SQL 语句
            for (const auto& statement : sql_statements) {
                stmt->execute(statement);
            }

            std::cout << "SQL script executed successfully" << std::endl;
        } catch (sql::SQLException& e) {
            std::cerr << "SQLException: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        }
    }
}

MySQLDao::~MySQLDao()
{
    _pool->close();
}

std::shared_ptr<UserInfo> MySQLDao::GetUser(int uid)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return nullptr;

    try
    {
        Defer defer([&conn, this] {
            _pool->returnConnection(std::move(conn));
        });

        // 准备调用存储过程
        std::unique_ptr <sql::PreparedStatement> pstmt(conn->_connection->prepareStatement("SELECT * FROM user WHERE uid = ?"));

        // 设置参数
        pstmt->setInt(1, uid);

        std::unique_ptr<sql::ResultSet> ret(pstmt->executeQuery());
        std::shared_ptr<UserInfo> user_ptr = nullptr;
        // 遍历结果集
        while (ret->next())
        {
            user_ptr.reset(new UserInfo);
            user_ptr->pwd = ret->getString("password");
            user_ptr->email = ret->getString("email");
            user_ptr->name= ret->getString("username");
            user_ptr->nick = ret->getString("nick");
            user_ptr->desc = ret->getString("desc");
            user_ptr->sex = ret->getInt("sex");
            user_ptr->icon = ret->getString("icon");
            user_ptr->uid = uid;
            break;
        }

        return user_ptr;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return nullptr;
    }
}

std::shared_ptr<UserInfo> MySQLDao::GetUser(std::string name)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return nullptr;

    try
    {
        Defer defer([&conn, this]
        {
            _pool->returnConnection(std::move(conn));
        });

        // 准备调用存储过程
        std::unique_ptr <sql::PreparedStatement> pstmt(conn->_connection->prepareStatement("SELECT * FROM user WHERE username = ?"));

        // 设置参数
        pstmt->setString(1, name);

        std::unique_ptr<sql::ResultSet> ret(pstmt->executeQuery());
        std::shared_ptr<UserInfo> user_ptr = nullptr;
        // 遍历结果集
        while (ret->next())
        {
            user_ptr.reset(new UserInfo);
            user_ptr->pwd = ret->getString("password");
            user_ptr->email = ret->getString("email");
            user_ptr->name= ret->getString("username");
            user_ptr->nick = ret->getString("nick");
            user_ptr->desc = ret->getString("desc");
            user_ptr->sex = ret->getInt("sex");
            user_ptr->icon = ret->getString("icon");
            user_ptr->uid = ret->getInt("uid");
            break;
        }

        return user_ptr;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return nullptr;
    }
}

std::vector<std::string> MySQLDao::splitSQLScript(const std::string& sql_content)
{
    std::vector<std::string> sql_statements;
    std::string content = sql_content; // 创建一个非 const 的副本
    size_t pos = 0;

    while ((pos = content.find(';')) != std::string::npos) {
        std::string statement = content.substr(0, pos);
        statement = Tools::Trim(statement); // 去除空白字符

        if (!statement.empty()) {
            sql_statements.push_back(statement);
        }

        content.erase(0, pos + 1); // 对副本进行操作
    }

    // 处理最后一条语句（如果没有分号结尾）
    std::string last_statement = Tools::Trim(content);
    if (!last_statement.empty()) {
        sql_statements.push_back(last_statement);
    }

    return sql_statements;
}
