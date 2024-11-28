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

    std::string query = "SELECT * FROM users WHERE username = '" + userName + "'";
    pqxx::result result = txn.exec(query);

    bool user_exists = false;
    for(auto row: result){
        user_exists = true;
    }

    if(user_exists){
        for(auto row: result){
            std::string salt = row["salt"].as<std::string>();
            if(row["password_hash"].as<std::string>() == hashing::hashPassword(password, salt).first){
                return true;
            }
        }
    }
    return false;
}

void SQL::addUser(const std::string& userName, const std::string& password){
    if (!conn || !conn->is_open()) throw std::runtime_error("No database connection");

    pqxx::work txn(*conn);

    std::string query = "SELECT username FROM users WHERE username = '" + userName + "';";
    bool userName_exists = false;
    pqxx::result result = txn.exec(query);
    for(auto row: result){
        userName_exists = true;
    }

    if(!userName_exists){
        std::pair<std::string, std::string> password_hash = hashing::hashPassword(password);
        txn.exec("INSERT INTO users (username, password_hash, salt) VALUES ('" + userName + "', '" + password_hash.first + "', '" + password_hash.second + "');");
        txn.commit();
    }
    else{
        throw std::out_of_range("User already exists");
    }
}

std::pair<bool, std::string> SQL::validateSession(const std::string& userId){
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec("SELECT * FROM sessions WHERE user_id = " + userId + " AND expires_at > CURRENT_TIMESTAMP");
    if(!result.empty()){
        return {true, result[0]["token"].c_str()};
    }
    return {false, "null"};
}

std::string SQL::addSession(const std::string& userId){
    const std::string token = hashing::generateToken();
    auto session = validateSession(userId);
    if(session.first){
        return ("Your session is already active\nThere is your token : " + session.second);
    }
    pqxx::work txn(*conn);
    txn.exec("INSERT INTO sessions (user_id, token) VALUES (" + userId + ", '" + token + "');");
    txn.commit();
    return token;
}

long long SQL::userId_num(const std::string& userName){
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec("SELECT id FROM users WHERE username = '" + userName + "';");
    for(auto row: result){
        return row["id"].as<long long>();
    }
    throw std::out_of_range("User " + userName + " doesn't exist");
}

std::string SQL::userId_str(const std::string& userName){
    pqxx::work txn(*conn);
    pqxx::result result = txn.exec("SELECT id FROM users WHERE username = '" + userName + "';");
    for(auto row: result){
        return row["id"].as<std::string>();
    }
    throw std::out_of_range("User " + userName + " doesn't exist");
}


void SQL::addFile(const std::string& fileName, const std::string& userId, const std::string& fileSize){
    pqxx::work txn(*conn);
    std::string query = "INSERT INTO files(user_id, filename, file_size, chunks_ids) VALUES (" + userId + ", '" + fileName + "', " + fileSize + " ," + "ARRAY[1,2,3]::BIGINT[]) RETURNING file_id;";
    pqxx::result result = txn.exec(query);
    long long fileId = result[0][0].as<long long>();
    std::cout << std::endl << std::endl << std::endl << std::endl << fileId << std::endl << std::endl << std::endl << std::endl;
    txn.commit();
}