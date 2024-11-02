#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <fstream>

void readChunk(std::ifstream& file, size_t chunkSize, size_t offset, std::vector<char>& buffer, size_t& bytesRead, std::mutex& readMutex);

std::vector<std::vector<char>> readFileByChunks(const std::string& filepath, size_t chunkSize);