#ifndef FILE_XOR_TEA_H
#define FILE_XOR_TEA_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Crypto {
    std::vector<uint8_t> readFile(const std::string& path);
    bool writeFile(const std::string& path, const std::vector<uint8_t>& data);
    bool fileExists(const std::string& path);
    uint64_t getFileSize(const std::string& path);
}

#endif
