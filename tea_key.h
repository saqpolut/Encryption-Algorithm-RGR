#ifndef TEA_KEY_H
#define TEA_KEY_H

#include <cstddef>

namespace Crypto {
    void generate_tea_key(char* out_key, size_t max_len);
}

#endif
