#pragma once

#include <vector>
#include <string>

void assembleFileFromChunks(const std::vector<std::vector<char>>& chunks, const std::string& destinationPath);
