#ifndef CIPHER_PLUGIN_H
#define CIPHER_PLUGIN_H

#include <stddef.h>

#ifdef _WIN32
    #ifdef PLUGIN_EXPORTING
        #define PLUGIN_API __declspec(dllexport)
    #else
        #define PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define PLUGIN_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

PLUGIN_API const char* plugin_get_name();
PLUGIN_API const char* plugin_get_key_hint();
PLUGIN_API int plugin_is_key_valid(const char* key);
PLUGIN_API void plugin_encrypt_text(const char* input, char* output, const char* key);
PLUGIN_API void plugin_decrypt_text(const char* input, char* output, const char* key);
PLUGIN_API void plugin_encrypt_block(const char* input, char* output, size_t size, const char* key);
PLUGIN_API void plugin_decrypt_block(const char* input, char* output, size_t size, const char* key);

#ifdef __cplusplus
}
#endif

#endif
