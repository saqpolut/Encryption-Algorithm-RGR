#include "plugin_loader_xor_tea.h"
#include "xor_tea_api.h"
#include <cstring>

static const char* current_algo = "XOR";

namespace Crypto {
    void load_plugin(const char* algo) {
        if (strcmp(algo, "XOR") == 0 || strcmp(algo, "TEA") == 0) {
            current_algo = algo;
        }
    }
    
    const char* get_plugin_name() {
        if (strcmp(current_algo, "XOR") == 0) {
            return xor_plugin_get_name();
        } else {
            return tea_plugin_get_name();
        }
    }
    
    void generate_key(char* out_key, size_t max_len) {
        if (strcmp(current_algo, "XOR") == 0) {
            xor_plugin_generate_key(out_key, max_len);
        } else {
            tea_plugin_generate_key(out_key, max_len);
        }
    }
    
    void process_data(const uint32_t* input, size_t len, const char* key, uint32_t* output) {
        if (strcmp(current_algo, "XOR") == 0) {
            xor_plugin_process_data(input, len, key, output);
        } else {
            tea_plugin_process_data(input, len, key, output);
        }
    }
}
