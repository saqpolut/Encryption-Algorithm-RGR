#ifndef RSA_KEY_H
#define RSA_KEY_H

#include <cstddef>

namespace rsa_keygen {
    void generate(char* out_key, size_t max_len);
}

#endif