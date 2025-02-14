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

bool MySQLDao::AddFriendApply(const int &from_uid, const int &to_uid)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this]
        {
            _pool->returnConnection(std::move(conn));
        });

        // 查询是否已经存在记录
        std::unique_ptr<sql::PreparedStatement> checkStmt(conn->_connection->prepareStatement(
            "SELECT COUNT(*) FROM friend_apply WHERE from_uid = ? AND to_uid = ?"));
        checkStmt->setInt(1, from_uid);
        checkStmt->setInt(2, to_uid);

        std::unique_ptr<sql::ResultSet> resultSet(checkStmt->executeQuery());
        if (resultSet->next() && resultSet->getInt(1) > 0)
        {
            // TODO
            // 记录已经存在，更新状态即可
            return true;
        }

        // 准备调用存储过程
        std::unique_ptr <sql::PreparedStatement> pstmt(conn->_connection->prepareStatement("INSERT INTO friend_apply (from_uid, to_uid) VALUES (?, ?)"));

        // 设置参数
        pstmt->setInt(1, from_uid);
        pstmt->setInt(2, to_uid);

        // 执行更新
        int rowAffected = pstmt->executeUpdate();
        if (rowAffected < 0) {
            return false;
        }

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

bool MySQLDao::AuthFriendApply(const int &from_uid, const int &to_uid)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this]
        {
            _pool->returnConnection(std::move(conn));
        });

        // 准备调用存储过程
        std::string sql = "UPDATE friend_apply SET status = 1 WHERE from_uid = ? AND to_uid = ?";
        std::unique_ptr <sql::PreparedStatement> pstmt(conn->_connection->prepareStatement(sql));

        // 设置参数
        pstmt->setInt(1, to_uid);
        pstmt->setInt(2, from_uid);

        // 执行更新
        int rowAffected = pstmt->executeUpdate();
        if (rowAffected < 0) {
            return false;
        }

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

bool MySQLDao::AddFriend(const int &from_uid, const int &to_uid, std::string back_name)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    Defer defer([&conn, this]
    {
        _pool->returnConnection(std::move(conn));
    });

    try
    {
        // 设置事务超时时间
        conn->_connection->setTransactionIsolation(sql::TRANSACTION_READ_COMMITTED);  // 添加这行
        std::unique_ptr<sql::Statement> stmt(conn->_connection->createStatement());
        stmt->execute("SET innodb_lock_wait_timeout = 50");  // 添加这行，设置锁等待超时时间

        // 开始事务
        conn->_connection->setAutoCommit(false);

        // 准备第一个SQL语句, 插入认证方好友数据
        std::string sql = "INSERT IGNORE INTO friend(self_id, friend_id, back) VALUES (?, ?, ?)";
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->_connection->prepareStatement(sql));
        //反过来的申请时from，验证时to
        pstmt->setInt(1, from_uid); // from id
        pstmt->setInt(2, to_uid);
        pstmt->setString(3, back_name);
        // 执行更新
        int rowAffected = pstmt->executeUpdate();
        if (rowAffected < 0)
        {
            conn->_connection->rollback();
            return false;
        }

        //准备第二个SQL语句，插入申请方好友数据
        std::unique_ptr<sql::PreparedStatement> pstmt2(conn->_connection->prepareStatement(sql));
        //反过来的申请时from，验证时to
        pstmt2->setInt(1, to_uid);
        pstmt2->setInt(2, from_uid);
        pstmt2->setString(3, back_name);
        // 执行更新
        int rowAffected2 = pstmt2->executeUpdate();
        if (rowAffected2 < 0)
        {
            conn->_connection->rollback();
            return false;
        }

        // 提交事务
        conn->_connection->commit();

        return true;
    }
    catch (sql::SQLException& e)
    {
        // 如果发生错误，回滚事务
        if (conn) {
            conn->_connection->rollback();
        }
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MySQLDao::GetApplyList(int self_id, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int offset, int limit)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this]
        {
            _pool->returnConnection(std::move(conn));
        });

        std::string sql = "select apply.from_uid, apply.status, user.username, "
        "user.nick, user.sex from friend_apply as apply join user on apply.from_uid = user.uid where apply.to_uid = ? "
        "and apply.id > ? order by apply.id ASC LIMIT ? ";

        std::unique_ptr<sql::PreparedStatement> pstmt(conn->_connection->prepareStatement(sql));
        pstmt->setInt(1, self_id); // 将uid替换为你要查询的uid
        pstmt->setInt(2, offset); // 起始id
        pstmt->setInt(3, limit); //偏移量
        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        // 遍历结果集
        while (res->next()) {
            auto name = res->getString("username");
            auto uid = res->getInt("from_uid");
            auto status = res->getInt("status");
            auto nick = res->getString("nick");
            auto sex = res->getInt("sex");
            auto apply_ptr = std::make_shared<ApplyInfo>(uid, name, "", "", nick, sex, status);
            applyList.push_back(apply_ptr);
        }

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
bool MySQLDao::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_info_list)
{
    auto conn = _pool->getConnection();
    if (conn == nullptr) return false;

    try
    {
        Defer defer([&conn, this]
        {
            _pool->returnConnection(std::move(conn));
        });

        std::string sql = "select * from friend where self_id = ? ";

        std::unique_ptr<sql::PreparedStatement> pstmt(conn->_connection->prepareStatement(sql));
        pstmt->setInt(1, self_id); // 将uid替换为你要查询的uid

        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        // 遍历结果集
        while (res->next()) {
            auto friend_id = res->getInt("friend_id");
            auto back = res->getString("back");
            //再一次查询friend_id对应的信息
            auto user_info = GetUser(friend_id);
            if (user_info == nullptr) {
                continue;
            }

            user_info->back = user_info->name;
            user_info_list.push_back(user_info);
        }

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
