#include "plugin_api.h"
#include <cstdlib>
#include <cstring>
#include <random>

using namespace std;

extern "C" {

PLUGIN_EXPORT const char* plugin_get_name() {
    return "XOR";
}

PLUGIN_EXPORT const char* plugin_get_key_format() {
    return "ключ длиной 8-256 байт (можно ввести как строку)";
}

PLUGIN_EXPORT void plugin_generate_key(char* out_key, size_t max_len) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(33, 126); // печатные символы
    
    size_t key_size = (max_len > 32) ? 32 : max_len - 1;
    for (size_t i = 0; i < key_size; ++i) {
        out_key[i] = static_cast<char>(dist(gen));
    }
    out_key[key_size] = '\0';
}

PLUGIN_EXPORT void plugin_process_data(const uint32_t* input, size_t len, 
                                       const char* key, uint32_t* output) {
    size_t key_len = strlen(key);
    if (key_len < 8) {
        throw runtime_error("Ошибка: Ключ XOR должен быть длиной 8-256 байт");
    }
    
    for (size_t i = 0; i < len; ++i) {
        uint32_t val = input[i];
        uint8_t* bytes = reinterpret_cast<uint8_t*>(&val);
        for (size_t j = 0; j < 4; ++j) {
            bytes[j] ^= key[(i * 4 + j) % key_len];
        }
        output[i] = val;
    }
}

} // extern "C"
