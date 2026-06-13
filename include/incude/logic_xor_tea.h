#ifndef LOGIC_XOR_TEA_H
#define LOGIC_XOR_TEA_H

#include <cstdint>
#include <cstddef>

namespace Crypto {
    void encrypt_data(const uint32_t* input, size_t len, const char* key, uint32_t* output, const char* algo);
    void decrypt_data(const uint32_t* input, size_t len, const char* key, uint32_t* output, const char* algo);
}

#endif
