#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <fstream>

class fileProcessing{
private:
    void readChunk(std::ifstream& file, size_t chunkSize, size_t offset, std::vector<char>& buffer, size_t& bytesRead, std::mutex& readMutex);
    void readAndAssembleChunk(std::ifstream& file, std::ofstream& destinationFile, size_t chunkSize, size_t offset, std::mutex& outputMutex);
public:
    std::vector<std::vector<char>> readFileByChunks(const std::string& filepath, size_t chunkSize);
    void assembleFileFromChunks(const std::vector<std::vector<char>>& chunks, const std::string& destinationPath);
    void processFile(const std::string& filepath, const std::string& destinationPath, size_t chunkSize);
};