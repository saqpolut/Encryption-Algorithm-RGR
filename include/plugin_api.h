#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include <cstdint>
#include <cstddef>

#ifdef _WIN32
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

PLUGIN_EXPORT const char* plugin_get_name();
PLUGIN_EXPORT const char* plugin_get_key_format();
PLUGIN_EXPORT void plugin_generate_key(char* out_key, size_t max_len);
PLUGIN_EXPORT void plugin_process_data(const uint32_t* input, size_t len, const char* key, uint32_t* output);

#ifdef __cplusplus
}
#endif

#endif