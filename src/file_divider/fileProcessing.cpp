#include "fileProcessing.hpp"

#include <sstream>
#include <thread>

namespace fileProcessing{
    namespace{
        void readChunk(std::ifstream& file, size_t chunkSize, size_t offset, std::vector<char>& buffer, size_t& bytesRead, std::mutex& readMutex) {
            std::lock_guard<std::mutex> lock(readMutex);
            file.seekg(offset);
            file.read(buffer.data(), chunkSize);
            bytesRead = file.gcount();
        }

        void readAndAssembleChunk(std::ifstream& file, std::ofstream& destinationFile, size_t chunkSize, size_t offset, std::mutex& outputMutex){
            std::lock_guard<std::mutex> lock(outputMutex);

            std::vector<char> buffer(chunkSize);
            size_t bytesRead;
            
            //read
            file.seekg(offset);
            file.read(buffer.data(), chunkSize);
            bytesRead = file.gcount(); 
            
            //write
            destinationFile.write(buffer.data(), bytesRead);
            file.close();
            destinationFile.close();
        }
    }   //namespace

    std::vector<std::vector<char>> readFileByChunks(const std::string& filepath, size_t chunkSize) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }

        size_t fileSize = file.tellg();
        std::mutex readMutex;
        size_t numChunks = (fileSize + chunkSize - 1) / chunkSize;
        std::vector<std::vector<char>> chunks(numChunks, std::vector<char>(chunkSize));
        std::vector<size_t> bytesRead(numChunks);

        std::vector<std::thread> threads;

        for (size_t i = 0; i < numChunks; ++i) {
            size_t offset = i * chunkSize;
            threads.emplace_back(readChunk, std::ref(file), chunkSize, offset, std::ref(chunks[i]), std::ref(bytesRead[i]), std::ref(readMutex));
        }

        for (auto& thread : threads) {
            thread.join();
        }

        for (size_t i = 0; i < chunks.size(); ++i) {
            chunks[i].resize(bytesRead[i]);
        }
        file.close();
        return chunks;
    }

    void assembleFileFromChunks(const std::vector<std::vector<char>>& chunks, const std::string& destinationPath) {
        std::ofstream destinationFile(destinationPath, std::ios::binary | std::ios::trunc);

        if (!destinationFile.is_open()) {
            throw std::runtime_error("Failed to open file");
        }

        for (size_t i = 0; i < chunks.size(); ++i) {
            destinationFile.write(chunks[i].data(), chunks[i].size());
        }

        destinationFile.close();
    }

    void processFile(const std::string& filepath, const std::string& destinationPath, size_t chunkSize){

        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }

        std::ofstream destinationFile(destinationPath, std::ios::binary | std::ios::trunc);
        if (!destinationFile.is_open()) {
            throw std::runtime_error("Failed to open file");
        } 

        size_t fileSize = file.tellg();
        std::mutex outputMutex;
        size_t numChunks = (fileSize + chunkSize - 1) / chunkSize;

        std::vector<std::thread> threads;

        for (size_t i = 0; i < numChunks; ++i) {
            size_t offset = i * chunkSize;
            threads.emplace_back(readAndAssembleChunk, std::ref(file), std::ref(destinationFile), chunkSize, offset, std::ref(outputMutex));
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

}   //namespace fileProcessing