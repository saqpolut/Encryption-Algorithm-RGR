#include "xor_key.h"
#include <random>

namespace Crypto {
    void generate_xor_key(char* out_key, size_t max_len) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(33, 126);
        
        size_t key_size = (max_len > 32) ? 32 : max_len - 1;
        for (size_t i = 0; i < key_size; ++i) {
            out_key[i] = static_cast<char>(dist(gen));
        }
        out_key[key_size] = '\0';
    }
}
