#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <fstream>

namespace fileProcessing{
    std::vector<std::vector<char>> readFileByChunks(const std::string& filepath, size_t chunkSize);
    void assembleFileFromChunks(const std::vector<std::vector<char>>& chunks, const std::string& destinationPath);
    void processFile(const std::string& filepath, const std::string& destinationPath, size_t chunkSize);
};