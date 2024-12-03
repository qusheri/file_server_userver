#include "session.hpp"

Session::Session(const std::string& user_token) {
    auto tokenValidation = db.validateToken(user_token);
    if(tokenValidation.first){
        throw "Invalid token or token has been expired\nSign in again\n";
    }
    session_id = tokenValidation.second[0];
    user_id = tokenValidation.second[1];
    token = tokenValidation.second[2];
    expires_at = tokenValidation.second[3];
    auto result = db.exec("SELECT username FROM users WHERE id = '" + username + "'");
    if(result.empty()){
        throw "Unknown user. Please, Sign up again";
    }
    username = result[0]["username"].as<std::string>();
}

