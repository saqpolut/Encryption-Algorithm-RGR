#include "plugin_api.h"
#include <cstdlib>
#include <cstring>
#include <random>

using namespace std;

static const uint32_t DELTA = 0x9E3779B9;
static const uint32_t ROUNDS = 32;

static void tea_encrypt_block(uint32_t* v, const uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0;
    for (uint32_t i = 0; i < ROUNDS; ++i) {
        sum += DELTA;
        v0 += ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        v1 += ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
    }
    v[0] = v0;
    v[1] = v1;
}

static void tea_decrypt_block(uint32_t* v, const uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1], sum = DELTA * ROUNDS;
    for (uint32_t i = 0; i < ROUNDS; ++i) {
        v1 -= ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        v0 -= ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        sum -= DELTA;
    }
    v[0] = v0;
    v[1] = v1;
}

extern "C" {

PLUGIN_EXPORT const char* plugin_get_name() {
    return "TEA";
}

PLUGIN_EXPORT const char* plugin_get_key_format() {
    return "16 байт (128 бит) - можно ввести как строку или hex";
}

PLUGIN_EXPORT void plugin_generate_key(char* out_key, size_t max_len) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(33, 126);
    
    for (int i = 0; i < 16 && i < static_cast<int>(max_len - 1); ++i) {
        out_key[i] = static_cast<char>(dist(gen));
    }
    out_key[16] = '\0';
}

PLUGIN_EXPORT void plugin_process_data(const uint32_t* input, size_t len, 
                                       const char* key, uint32_t* output) {
    size_t key_len = strlen(key);
    if (key_len != 16) {
        throw runtime_error("Ошибка: Ключ TEA должен быть 16 байт");
    }
    
    // Распаковка ключа в 4x32 бита
    uint32_t k[4];
    for (int i = 0; i < 4; ++i) {
        k[i] = (uint32_t)(unsigned char)key[i*4] |
               ((uint32_t)(unsigned char)key[i*4+1] << 8) |
               ((uint32_t)(unsigned char)key[i*4+2] << 16) |
               ((uint32_t)(unsigned char)key[i*4+3] << 24);
    }
    
    // Обработка блоков по 2 uint32_t (8 байт)
    size_t blocks = len / 2;
    for (size_t i = 0; i < blocks; ++i) {
        uint32_t v[2] = { input[i*2], input[i*2+1] };
        tea_encrypt_block(v, k);
        output[i*2] = v[0];
        output[i*2+1] = v[1];
    }
    
    // Если нечётное количество uint32_t, копируем без изменений
    if (len % 2 == 1) {
        output[len-1] = input[len-1];
    }
}

} // extern "C"
