#pragma once
#include <string>
#include "../db_funcs/sql_class.hpp"

class Session {
public:
    Session(const std::string& token);
    ~Session() = default;
    std::string token;
    std::string user_id;
    std::string expires_at;
    std::string username;
    std::string session_id;

private:
    SQL db;
};
