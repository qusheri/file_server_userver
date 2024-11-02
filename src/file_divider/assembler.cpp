#include "assembler.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

void assembleFileFromChunks(const std::vector<std::vector<char>>& chunks, const std::string& destinationPath) {
    std::ofstream destinationFile(destinationPath, std::ios::trunc);

    if (!destinationFile.is_open()) {
        std::cerr << "Can't create file: " << destinationPath << std::endl;
        return;
    }

    for (size_t i = 0; i < chunks.size(); ++i) {
        destinationFile.write(chunks[i].data(), chunks[i].size());
        std::cout << "Written chunk " << i + 1 << ", size: " << chunks[i].size() << " bytes\n";
    }

    destinationFile.close();
    std::cout << "File succesfully builded!\n";
}