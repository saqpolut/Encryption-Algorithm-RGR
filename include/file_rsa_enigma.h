#ifndef FILE_RSA_ENIGMA_H
#define FILE_RSA_ENIGMA_H

#include <string>
#include <vector>
#include <cstdint>

namespace file {
    std::vector<uint8_t> read_binary_file(const std::string& file_path);
    void write_encrypted_file(const std::string& file_path, const std::vector<uint32_t>& data);
    std::vector<uint32_t> read_encrypted_file(const std::string& file_path);
    void write_decrypted_file(const std::string& file_path, const std::vector<uint32_t>& data);
}

#endif