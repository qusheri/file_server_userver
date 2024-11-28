#pragma once
#include <string>
#include <pqxx/pqxx>

class SQL {
public:
    SQL(const std::string& config_file);
    SQL();
    ~SQL();

    void connect();
    void createTable(const std::string& tableName, const std::string& columns);
    void insertRow(const std::string& tableName, const std::string& values);
    bool validateUser(const std::string& id, const std::string& password_hash);
    void addUser(const std::string& userName, const std::string& password);
    std::pair<bool, std::string> validateSession(const std::string& userId);
    std::string addSession(const std::string& userId);
    long long userId_num(const std::string& userName);
    std::string userId_str(const std::string& userName);
    void addFile(const std::string& fileName, const std::string& userId, const std::string& fileSize);

private:
    std::string db_host = std::getenv("DB_HOST");
    std::string db_port = std::getenv("DB_PORT");
    std::string db_name = "postgres";
    std::string db_user = std::getenv("DB_USER");
    std::string db_password = std::getenv("DB_PASSWORD");

    pqxx::connection* conn;

    void loadConfig(const std::string& config_file);
};
