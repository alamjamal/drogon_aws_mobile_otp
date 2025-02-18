#include "DatabaseManager.h"
#include <iostream>
#include <thread>

// Connection class implementation
Connection::Connection(const std::string& host, const std::string& user, const std::string& password, const std::string& database, const unsigned int& port)
    : conn(mysql_init(nullptr), mysql_close)
{
    mysql_real_connect(conn.get(), host.c_str(), user.c_str(), password.c_str(), database.c_str(),port, nullptr, 0);
}

Connection::~Connection()
{
    mysql_close(conn.get());
}

MYSQL* Connection::getConnection()
{
    return conn.get();
}

bool Connection::isConnected() const
{
    return mysql_ping(conn.get()) == 0;
}

void Connection::reconnect(const std::string& host, const std::string& user, const std::string& password, const std::string& database)
{
    if (!isConnected())
    {
        mysql_real_connect(conn.get(), host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, nullptr, 0);
    }
}

// ConnectionPool class implementation
ConnectionPool& ConnectionPool::getInstance()
{
    static ConnectionPool instance;
    return instance;
}

ConnectionPool::ConnectionPool()
    : host(Config::getEnvVar("DB_HOST", "127.0.0.1")), user(Config::getEnvVar("DB_USER", "root")), password(Config::getEnvVar("DB_PASSWORD", "root")),port(std::stoi(Config::getEnvVar("DB_PORT", "3306"))) ,database(Config::getEnvVar("DB_NAME", ""))
{
    createConnections();
}

ConnectionPool::~ConnectionPool()
{
    while (!availableConnections.empty())
    {
        auto& it = availableConnections.front();
        mysql_close(it->getConnection());
    }
    
}

void ConnectionPool::createConnections()
{
    // Create a fixed number of connections for the pool
    for (int i = 0; i < 4; ++i)
    {
        auto conn = std::make_shared<Connection>(host, user, password, database,port);
        connections.push_back(conn);
        availableConnections.push(conn);
    }
}

std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] { return !availableConnections.empty(); });

    auto conn = availableConnections.front();
    availableConnections.pop();

    return conn;
}

void ConnectionPool::releaseConnection(std::shared_ptr<Connection> conn)
{
    std::unique_lock<std::mutex> lock(mtx);
    availableConnections.push(conn);
    cv.notify_one();
}

// DatabaseManager class implementation
DatabaseManager& DatabaseManager::getInstance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
    : connectionPool(std::make_shared<ConnectionPool>())
{
    std::cout<<"Database ConnectionPool has been created..."<<std::endl;
}

DatabaseManager::~DatabaseManager()
{
}

void DatabaseManager::executeQuery(const std::string& query)
{
    auto conn = connectionPool->getConnection();
    if (!conn->isConnected())
    {
        reconnect(conn);
    }

    if (mysql_query(conn->getConnection(), query.c_str()))
    {
        // Handle query error
        std::cerr << "Query execution failed: " << mysql_error(conn->getConnection()) << std::endl;
        return;
    }
    std::cout<<"Query executed..."<<std::endl;
    connectionPool->releaseConnection(conn);
}

void DatabaseManager::executeAsyncQuery(const std::string& query)
{
    std::thread([this, query]() {
        executeQuery(query);
    }).detach();
}

void DatabaseManager::reconnect(std::shared_ptr<Connection> conn)
{
    conn->reconnect(connectionPool->host, connectionPool->user, connectionPool->password, connectionPool->database);
}
