#include "plugin_api.h"
#include "rsa_key.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>

static uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return res;
}

extern "C" {

PLUGIN_EXPORT const char* plugin_get_name() {
    return "RSA"; 
}
PLUGIN_EXPORT const char* plugin_get_key_format() {
    return "n,e (для шифрования) или n,d (для дешифрования)"; 
}

PLUGIN_EXPORT void plugin_generate_key(char* out_key, size_t max_len) {
    rsa_keygen::generate(out_key, max_len);
}

PLUGIN_EXPORT void plugin_process_data(const uint32_t* input, size_t len, const char* key, uint32_t* output) {
    char key_copy[128];
    std::strncpy(key_copy, key, sizeof(key_copy) - 1);
    key_copy[sizeof(key_copy) - 1] = '\0';

    char* token = std::strtok(key_copy, ",");
    if (!token) throw std::runtime_error("Ошибка: Неверный формат ключа RSA.");
    uint64_t n = std::strtoull(token, nullptr, 10);

    token = std::strtok(nullptr, ",");
    if (!token) throw std::runtime_error("Ошибка: Неверный формат ключа RSA.");
    uint64_t exp = std::strtoull(token, nullptr, 10);

    if (n <= 1) {
        throw std::runtime_error("Ошибка: Модуль (n) должен быть > 1.");
    }

    for (size_t i = 0; i < len; ++i) {
        output[i] = static_cast<uint32_t>(mod_pow(input[i], exp, n));
    }
}

}