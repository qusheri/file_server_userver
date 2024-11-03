#include "reader.hpp"

#include <iostream>
#include <sstream>
#include <thread>

void readChunk(std::ifstream& file, size_t chunkSize, size_t offset, std::vector<char>& buffer, size_t& bytesRead, std::mutex& readMutex) {
    std::lock_guard<std::mutex> lock(readMutex);
    file.seekg(offset);
    file.read(buffer.data(), chunkSize);
    bytesRead = file.gcount();

    std::cout << "Chunk written, offser: " << offset << ", bytes: " << bytesRead << '\n';
}

std::vector<std::vector<char>> readFileByChunks(const std::string& filepath, size_t chunkSize) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Can't open the " << filepath << std::endl;
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