#ifndef XOR_KEY_H
#define XOR_KEY_H

#include <cstddef>

namespace Crypto {
    void generate_xor_key(char* out_key, size_t max_len);
}

#endif
