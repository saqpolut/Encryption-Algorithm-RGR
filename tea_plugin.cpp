#include "xor_tea_api.h"
#include <cstring>
#include <random>

using namespace std;

static const uint32_t DELTA = 0x9E3779B9;
static const uint32_t ROUNDS = 32;

static void tea_encrypt(uint32_t* v, const uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0;
    for (uint32_t i = 0; i < ROUNDS; ++i) {
        sum += DELTA;
        v0 += ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        v1 += ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
    }
    v[0] = v0; v[1] = v1;
}

extern "C" {

PLUGIN_EXPORT const char* tea_plugin_get_name() {
    return "TEA";
}

PLUGIN_EXPORT const char* tea_plugin_get_key_format() {
    return "16 bytes";
}

PLUGIN_EXPORT void tea_plugin_generate_key(char* out_key, size_t max_len) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(33, 126);
    
    for (int i = 0; i < 16 && i < (int)max_len - 1; ++i) {
        out_key[i] = static_cast<char>(dist(gen));
    }
    out_key[16] = '\0';
}

PLUGIN_EXPORT void tea_plugin_process_data(const uint32_t* input, size_t len, 
                                           const char* key, uint32_t* output) {
    uint32_t k[4];
    for (int i = 0; i < 4; ++i) {
        k[i] = (uint32_t)(unsigned char)key[i*4] |
               ((uint32_t)(unsigned char)key[i*4+1] << 8) |
               ((uint32_t)(unsigned char)key[i*4+2] << 16) |
               ((uint32_t)(unsigned char)key[i*4+3] << 24);
    }
    
    for (size_t i = 0; i < len / 2; ++i) {
        uint32_t v[2] = { input[i*2], input[i*2+1] };
        tea_encrypt(v, k);
        output[i*2] = v[0];
        output[i*2+1] = v[1];
    }
    if (len % 2 == 1) output[len-1] = input[len-1];
}

}
