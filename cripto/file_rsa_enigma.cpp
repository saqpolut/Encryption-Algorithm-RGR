#include "file_rsa_enigma.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace file {

std::vector<uint8_t> read_binary_file(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для чтения: " + file_path);
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Ошибка чтения данных из файла: " + file_path);
    }
    return buffer;
}

void write_encrypted_file(const std::string& file_path, const std::vector<uint32_t>& data) {
    std::filesystem::path path(file_path);
    if (path.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);
        if (ec) {
            throw std::runtime_error("Не удалось создать директории: " + path.parent_path().string());
        }
    }

    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось создать файл для записи: " + file_path);
    }
    
    file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(uint32_t));
    if (!file.good()) {
        throw std::runtime_error("Ошибка записи данных в файл: " + file_path);
    }
}

std::vector<uint32_t> read_encrypted_file(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для чтения: " + file_path);
    }
    
    std::streamsize size_bytes = file.tellg();
    file.seekg(0, std::ios::beg);
    
    size_t num_elements = size_bytes / sizeof(uint32_t);
    std::vector<uint32_t> buffer(num_elements);
    
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size_bytes)) {
        throw std::runtime_error("Ошибка чтения данных из файла: " + file_path);
    }
    return buffer;
}

void write_decrypted_file(const std::string& file_path, const std::vector<uint32_t>& data) {
    std::filesystem::path path(file_path);
    if (path.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(path.parent_path(), ec);
        if (ec) {
            throw std::runtime_error("Не удалось создать директории: " + path.parent_path().string());
        }
    }

    std::ofstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось создать файл для записи: " + file_path);
    }
    
    std::vector<uint8_t> bytes(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        bytes[i] = static_cast<uint8_t>(data[i] & 0xFF);
    }
    
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    if (!file.good()) {
        throw std::runtime_error("Ошибка записи данных в файл: " + file_path);
    }
}

}