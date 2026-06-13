#include "tea_key.h"
#include <random>

namespace Crypto {
    void generate_tea_key(char* out_key, size_t max_len) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(33, 126);
        
        for (int i = 0; i < 16 && i < (int)max_len - 1; ++i) {
            out_key[i] = static_cast<char>(dist(gen));
        }
        out_key[16] = '\0';
    }
}
