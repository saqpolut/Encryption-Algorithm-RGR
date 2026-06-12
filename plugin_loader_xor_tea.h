#ifndef PLUGIN_LOADER_XOR_TEA_H
#define PLUGIN_LOADER_XOR_TEA_H

#include <cstddef>
#include<cstdint>

namespace Crypto {
    void load_plugin(const char* algo);
    const char* get_plugin_name();
    void generate_key(char* out_key, size_t max_len);
    void process_data(const uint32_t* input, size_t len, const char* key, uint32_t* output);
}

#endif
