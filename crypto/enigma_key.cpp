#include "enigma_key.h"
#include <random>

namespace enigma_keygen {

void generate(char* out_key, size_t max_len) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 25); 
    
    if (max_len >= 4) {
        out_key[0] = static_cast<char>('A' + dis(gen));
        out_key[1] = static_cast<char>('A' + dis(gen));
        out_key[2] = static_cast<char>('A' + dis(gen));
        out_key[3] = '\0';
    }
}

}