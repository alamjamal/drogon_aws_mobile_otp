#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <memory>
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <mariadb/mysql.h>

#include "Config.h"

class Connection
{
public:
    Connection(const std::string& host, const std::string& user, const std::string& password, const std::string& database, const unsigned int& port);
    ~Connection();

    MYSQL* getConnection();
    bool isConnected() const;
    void reconnect(const std::string& host, const std::string& user, const std::string& password, const std::string& database);

private:
    std::unique_ptr<MYSQL, void(*)(MYSQL*)> conn;
};

class ConnectionPool
{
public:
    static ConnectionPool& getInstance();
    
    std::shared_ptr<Connection> getConnection();
    void releaseConnection(std::shared_ptr<Connection> conn);

    ConnectionPool();
    ~ConnectionPool();
    std::string host;
    std::string user;
    std::string password;
    std::string database;
    unsigned int port;
private:
    void createConnections();

    std::vector<std::shared_ptr<Connection>> connections;
    std::queue<std::shared_ptr<Connection>> availableConnections;
    std::mutex mtx;
    std::condition_variable cv;
};

class DatabaseManager
{
public:
    static DatabaseManager& getInstance();

    void executeQuery(const std::string& query);
    void executeAsyncQuery(const std::string& query);

private:
    DatabaseManager();
    ~DatabaseManager();

    void reconnect(std::shared_ptr<Connection> conn);

    std::shared_ptr<ConnectionPool> connectionPool;
};

#endif
