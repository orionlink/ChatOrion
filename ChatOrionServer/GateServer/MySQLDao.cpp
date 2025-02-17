//
// Created by hwk on 2025/1/7.
//

#include "MySQLDao.h"
#include "Settings.h"
#include "tools.h"
#include "log.h"

MySQLDao::MySQLDao()
{
}

MySQLDao::~MySQLDao()
{
    _pool->close();
}

bool MySQLDao::init()
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
    std::string sql_procedure = Tools::ReadFile("reg_user_procedure.sql");
    if (!sql_content.empty())
    {
        try
        {
            auto conn = _pool->getConnection();
            Defer defer([&conn, this] {
                if (conn != nullptr) _pool->returnConnection(std::move(conn));
            });

            std::unique_ptr<sql::Statement> stmt(conn->_connection->createStatement());

            stmt->execute("DROP PROCEDURE IF EXISTS reg_user");
            stmt->execute(sql_procedure);

            // 拆分 SQL 脚本为多个语句
            std::vector<std::string> sql_statements = splitSQLScript(sql_content);

            // 逐条执行 SQL 语句
            for (const auto& statement : sql_statements) {
                stmt->execute(statement);
            }

            LOG_INFO << "SQL script executed successfully";
            return true;
        } catch (sql::SQLException& e) {
            std::cerr << "SQLException: " << e.what();
            std::cerr << " (MySQL error code: " << e.getErrorCode();
            std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
            return false;
        }
    }
    else
    {
        return false;
    }
}

int MySQLDao::registerUser(const std::string &name, const std::string &email, const std::string &pwd)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this] {
            _pool->returnConnection(std::move(conn));
        });

        // 准备调用存储过程
        std::unique_ptr <sql::PreparedStatement> stmt(conn->_connection->prepareStatement("CALL reg_user(?,?,?,@result,@error_info)"));

        // 设置参数
        stmt->setString(1, name);
        stmt->setString(2, email);
        stmt->setString(3, pwd);

        // 由于PreparedStatement不直接支持注册输出参数，我们需要使用会话变量或其他方法来获取输出参数的值
        // 执行存储过程
        stmt->execute();

        std::unique_ptr <sql::Statement> stmtResult(conn->_connection->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result, @error_info AS error_info"));
        if (res->next())
        {
            int result = res->getInt("result");
            std::string error_info = res->getString("error_info");
            std::cout << "Result: " << result << std::endl;
            if (result == -1) std::cout << "error_info: " << error_info << std::endl;
            return result;
        }

        return -1;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1;
    }
}

bool MySQLDao::checkUsernameEmailMatch(const std::string &name, const std::string &email)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this] {
            _pool->returnConnection(std::move(conn));
        });

        // 准备调用存储过程
        std::unique_ptr <sql::PreparedStatement> pstmt(conn->_connection->prepareStatement("SELECT username, email FROM user WHERE username = ?"));

        pstmt->setString(1, name);

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 判断是否有结果
        while (res->next()) {
            // 获取结果集中的数据
            std::string dbUsername = res->getString("username");
            std::string dbEmail = res->getString("email");

            // 判断是否完全匹配
            if (dbUsername == name && dbEmail == email) {
                return true; // 完全匹配
            }
        }

        // 如果没有结果或不匹配
        return false;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MySQLDao::updatePassword(const std::string &name, const std::string &newpwd)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this] {
            _pool->returnConnection(std::move(conn));
        });

        // 准备调用存储过程
        std::unique_ptr <sql::PreparedStatement> pstmt(conn->_connection->prepareStatement("UPDATE user SET password = ? WHERE username = ?"));

        pstmt->setString(2, name);
        pstmt->setString(1, newpwd);

        // 执行查询
        int updateCount = pstmt->executeUpdate();

        if (updateCount > 0) {
            std::cout << "Rows affected: " << updateCount << std::endl;
            return true;
        } else if (updateCount == 0) {
            std::cout << "No rows affected." << std::endl;
            return true;
        } else {
            std::cout << "No row count available." << std::endl;
            return false;
        }
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

int MySQLDao::checkUserLogin(const std::string &name, const std::string &pwd, UserInfo &userInfo)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this] {
            _pool->returnConnection(std::move(conn));
        });

        // 准备调用存储过程
        std::unique_ptr <sql::PreparedStatement> pstmt(conn->_connection->prepareStatement("SELECT * FROM user WHERE username = ?"));

        pstmt->setString(1, name);

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) return 1;

        std::string origin_pwd = res->getString("password");

        if (origin_pwd != pwd) return 2;

        userInfo.name = name;
        userInfo.pwd = origin_pwd;
        userInfo.email = res->getString("email");
        userInfo.uid = res->getInt("uid");

        return 0;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return -1;
    }
}

bool MySQLDao::checkEmailLoginWithCode(const std::string &email, UserInfo &userInfo)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this] {
            _pool->returnConnection(std::move(conn));
        });

        // 准备调用存储过程
        std::unique_ptr <sql::PreparedStatement> pstmt(conn->_connection->prepareStatement("SELECT * FROM user WHERE email = ?"));

        pstmt->setString(1, email);

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (!res->next()) return false;

        std::string origin_email = res->getString("email");

        if (origin_email != email) return false;

        userInfo.name = res->getString("username");
        userInfo.pwd = res->getString("password");
        userInfo.email = origin_email;
        userInfo.uid = res->getInt("uid");

        return true;
    }
    catch (sql::SQLException& e)
    {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
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
