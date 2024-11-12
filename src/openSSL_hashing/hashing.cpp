#include "hashing.hpp"

#include <vector>
#include <iomanip>
#include <stdexcept>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>

namespace hashing{
    namespace{
        std::string toHex(const std::vector<unsigned char>& data) {
            std::ostringstream oss;
            for (unsigned char byte : data) {
                oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            }
            return oss.str();
        }

        std::vector<unsigned char> generateSalt(int length = 16) {
            std::vector<unsigned char> salt(length);
            if (RAND_bytes(salt.data(), length) != 1) {
                throw std::runtime_error("Failed to generate salt");
            }
            return salt;
        }
    }   //namespace
    std::string hashFileName(const std::string& original_name, int user_id, const std::string& timestamp) {
        std::string input = original_name + ".." + std::to_string(user_id) + timestamp;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }
    
    std::pair<std::string, std::string> hashPassword(const std::string& password, const std::string& salt) {
        int iterations = 5000;
        const int key_length = 32; // Длина итогового хеша в байтах
        std::vector<unsigned char> hash(key_length);

        if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                            reinterpret_cast<const unsigned char*>(salt.c_str()), salt.size(),
                            iterations, EVP_sha256(), key_length, hash.data()) != 1) {
            throw std::runtime_error("Failed to hash password with PBKDF2");
        }

        return {toHex(hash), salt};
    }

    std::pair<std::string, std::string> hashPassword(const std::string& password) {
        const std::string& salt = toHex(generateSalt());
        int iterations = 5000;
        const int key_length = 32; // Длина итогового хеша в байтах
        std::vector<unsigned char> hash(key_length);

        if (PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
                            reinterpret_cast<const unsigned char*>(salt.c_str()), salt.size(),
                            iterations, EVP_sha256(), key_length, hash.data()) != 1) {
            throw std::runtime_error("Failed to hash password with PBKDF2");
        }

        return {toHex(hash), salt};
    }

    std::string generateToken(){
        int length = 32;
        std::vector<unsigned char> buffer(length);
        RAND_bytes(buffer.data(), length);

        return toHex(buffer);
    }

}   //namespace hashing