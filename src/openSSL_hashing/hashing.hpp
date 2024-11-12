#include <sstream>

namespace hashing{

    std::string hashFileName(const std::string& original_name, int user_id, const std::string& timestamp);
    
    std::pair<std::string, std::string> hashPassword(const std::string& password, const std::string& salt);
    std::pair<std::string, std::string> hashPassword(const std::string& password);

    std::string generateToken();

}   //namespace hashing