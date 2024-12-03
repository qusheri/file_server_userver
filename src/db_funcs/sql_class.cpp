#include "sql_class.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <openssl/sha.h>

#include "../openSSL_hashing/hashing.hpp"

SQL::SQL(const std::string& config_file) {
    loadConfig(config_file);
    connect();
}

SQL::SQL() {
    connect();
}

SQL::~SQL() {
    if (conn) {
        conn->close();
        delete conn;
    }
}

void SQL::loadConfig(const std::string& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) throw std::runtime_error("Failed to open config file");

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            if (key == "DB_HOST") db_host = value;
            else if (key == "DB_PORT") db_port = value;
            else if (key == "DB_NAME") db_name = value;
            else if (key == "DB_USER") db_user = value;
            else if (key == "DB_PASSWORD") db_password = value;
        }
    }
}

void SQL::connect() {
    try {
        std::string conn_str = "host=" + db_host + " port=" + db_port + 
                               " dbname=" + db_name + " user=" + db_user + 
                               " password=" + db_password;
        conn = new pqxx::connection(conn_str);
        if (!conn->is_open()) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Connection error: ") + e.what());
    }
}

void SQL::createTable(const std::string& tableName, const std::string& columns) {
    if (!conn || !conn->is_open()) throw std::runtime_error("No database connection");

    pqxx::work txn(*conn);
    std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (" + columns + ");";
    txn.exec(query);
    txn.commit();
}

void SQL::insertRow(const std::string& tableName, const std::string& values) {
    if (!conn || !conn->is_open()) throw std::runtime_error("No database connection");

    pqxx::work txn(*conn);
    std::string query = "INSERT INTO " + tableName + " VALUES (" + values + ");";
    txn.exec(query);
    txn.commit();
}

bool SQL::validateUser(const std::string& userName, const std::string& password){
    if (!conn || !conn->is_open()) throw std::runtime_error("No database connection");

    pqxx::work txn(*conn);

    pqxx::result result = txn.exec_params("SELECT password_hash, salt FROM users WHERE username = $1", userName);

    if (result.empty()) return false;

    std::string db_password_hash = result[0]["password_hash"].as<std::string>();
    std::string salt = result[0]["salt"].as<std::string>();

    return db_password_hash == hashing::hashPassword(password, salt).first;
}

void SQL::addUser(const std::string& userName, const std::string& password){
    if (!conn || !conn->is_open()) throw std::runtime_error("No database connection");

    pqxx::work txn(*conn);

    pqxx::result result = txn.exec_params("SELECT username FROM users WHERE username = $1", userName);
    if (!result.empty()) {
        throw std::out_of_range("User already exists");
    }

    auto [password_hash, salt] = hashing::hashPassword(password);

    txn.exec_params("INSERT INTO users (username, password_hash, salt) VALUES ($1, $2, $3)",
                    userName, password_hash, salt);
    txn.commit();
}

std::pair<bool, std::string> SQL::validateSession(const std::string& userId){
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(
        "SELECT token FROM sessions WHERE user_id = $1 AND expires_at > CURRENT_TIMESTAMP", userId);
    if (!result.empty()) {
        return {true, result[0]["token"].as<std::string>()};
    }
    return {false, "null"};
}

std::pair<bool, std::vector<std::string>> SQL::validateToken(const std::string& token){
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params("SELECT * FROM sessions WHERE token = $1 AND expires_at > CURRENT_TIMESTAMP", token);
    if(result.empty()){
        return {false, {}};
    }
    std::vector<std::string> results;
    for (auto res: result){
        results.push_back(res["session_id"].as<std::string>());
        results.push_back(res["user_id"].as<std::string>());
        results.push_back(res["token"].as<std::string>());
        results.push_back(res["expires_at"].as<std::string>());
    }
    return {true, results};
}

std::string SQL::addSession(const std::string& userId){
    const std::string token = hashing::generateToken();

    auto session = validateSession(userId);
    if (session.first) {
        return "Your session is already active\nThere is your token : " + session.second;
    }

    pqxx::work txn(*conn);
    txn.exec_params("INSERT INTO sessions (user_id, token) VALUES ($1, $2)", userId, token);
    txn.commit();
    return token;
}

long long SQL::userId_num(const std::string& userName){
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params("SELECT id FROM users WHERE username = $1", userName);
    if (!result.empty()) {
        return result[0]["id"].as<long long>();
    }
    throw std::out_of_range("User " + userName + " doesn't exist");
}

std::string SQL::userId_str(const std::string& userName){
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params("SELECT id FROM users WHERE username = $1", userName);
    if (!result.empty()) {
        return result[0]["id"].as<std::string>();
    }
    throw std::out_of_range("User " + userName + " doesn't exist");
}

void SQL::addFile(const std::string& fileName, const std::string& userId, const std::string& fileSize){
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec_params(
        "INSERT INTO files(user_id, filename, file_size, chunks_ids) "
        "VALUES ($1, $2, $3, ARRAY[1,2,3]::BIGINT[]) RETURNING file_id",
        userId, fileName, fileSize);
    long long fileId = result[0]["file_id"].as<long long>();
    txn.commit();
}

pqxx::result SQL::exec(const std::string& command){
    if (!conn || !conn->is_open()) {
        throw std::runtime_error("No database connection");
    }
    pqxx::work txn(*conn);
    return txn.exec(command);
}