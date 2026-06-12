#include "cipher_plugin.h"
#include "skitala_impl.h"
#include <stdlib.h>
#include <string.h>

static int getColumns(const char* key) {
    if (!key || key[0] == '\0') return 3;
    int cols = atoi(key);
    if (cols < 2) return 2;
    if (cols > 20) return 20;
    return cols;
}

PLUGIN_API const char* plugin_get_name() {
    return "Скитала";
}

PLUGIN_API const char* plugin_get_key_hint() {
    return "Подсказка: ключ — число столбцов от 2 до 20 (например, 5)";
}

PLUGIN_API int plugin_is_key_valid(const char* key) {
    if (!key || key[0] == '\0') return 0;
    int cols = atoi(key);
    return (cols >= 2 && cols <= 20);
}

PLUGIN_API void plugin_encrypt_text(const char* input, char* output, const char* key) {
    skitalaEncrypt(input, output, getColumns(key));
}

PLUGIN_API void plugin_decrypt_text(const char* input, char* output, const char* key) {
    skitalaDecrypt(input, output, getColumns(key));
}

PLUGIN_API void plugin_encrypt_block(const char* input, char* output, size_t size, const char* key) {
    for (size_t i = 0; i < size; i++) output[i] = input[i];
}

PLUGIN_API void plugin_decrypt_block(const char* input, char* output, size_t size, const char* key) {
    for (size_t i = 0; i < size; i++) output[i] = input[i];
}
