#include "logic_xor_tea.h"
#include "xor_plugin.cpp"
#include "tea_plugin.cpp"
#include <cstring>

namespace Crypto {
    void encrypt_data(const uint32_t* input, size_t len, const char* key, uint32_t* output, const char* algo) {
        if (strcmp(algo, "XOR") == 0) {
            plugin_process_data(input, len, key, output);
        } else if (strcmp(algo, "TEA") == 0) {
            plugin_process_data(input, len, key, output);
        }
    }
    
    void decrypt_data(const uint32_t* input, size_t len, const char* key, uint32_t* output, const char* algo) {
        encrypt_data(input, len, key, output, algo); // XOR и TEA симметричны
    }
}
