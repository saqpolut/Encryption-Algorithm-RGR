#include "logic_xor_tea.h"
#include "xor_tea_api.h"
#include <cstring>

namespace Crypto {
    void encrypt_data(const uint32_t* input, size_t len, const char* key, uint32_t* output, const char* algo) {
        if (strcmp(algo, "XOR") == 0) {
            xor_plugin_process_data(input, len, key, output);
        } else if (strcmp(algo, "TEA") == 0) {
            tea_plugin_process_data(input, len, key, output);
        }
    }
    
    void decrypt_data(const uint32_t* input, size_t len, const char* key, uint32_t* output, const char* algo) {
        encrypt_data(input, len, key, output, algo);
    }
}
