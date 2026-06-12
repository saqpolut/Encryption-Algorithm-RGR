#ifndef FILE_XOR_TEA_H
#define FILE_XOR_TEA_H

#include <cstdint>
#include <cstddef>

namespace Crypto {
    void process_file(const char* input_path, const char* output_path,
                      const char* key, bool encrypt);
}

#endif
