#include "cipher_plugin.h"
#include "vigener_impl.h"
#include <cstring>

PLUGIN_API const char* plugin_get_name() {
    return "Виженер";
}

PLUGIN_API const char* plugin_get_key_hint() {
    return "Подсказка: ключ — любое слово (буквы, цифры, символы)";
}

PLUGIN_API int plugin_is_key_valid(const char* key) {
    return (key && key[0] != '\0');
}

PLUGIN_API void plugin_encrypt_text(const char* input, char* output, const char* key) {
    vigenerEncrypt(input, output, key);
}

PLUGIN_API void plugin_decrypt_text(const char* input, char* output, const char* key) {
    vigenerDecrypt(input, output, key);
}

PLUGIN_API void plugin_encrypt_block(const char* input, char* output, size_t size, const char* key) {
    vigenerEncryptBlock(input, output, size, key);
}

PLUGIN_API void plugin_decrypt_block(const char* input, char* output, size_t size, const char* key) {
    vigenerDecryptBlock(input, output, size, key);
}
