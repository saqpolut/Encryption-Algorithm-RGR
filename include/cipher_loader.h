#ifndef CIPHER_LOADER_H
#define CIPHER_LOADER_H

#include "cipher_plugin.h"

#define MAX_PLUGINS 10

struct LoadedPlugin {
    void* handle;
    const char* (*get_name)();
    const char* (*get_key_hint)();
    int (*is_key_valid)(const char*);
    void (*encrypt_text)(const char*, char*, const char*);
    void (*decrypt_text)(const char*, char*, const char*);
    void (*encrypt_block)(const char*, char*, size_t, const char*);
    void (*decrypt_block)(const char*, char*, size_t, const char*);
};

int loadAllPlugins();
int getPluginsCount();
LoadedPlugin* getPlugin(int index);
void unloadAllPlugins();

#endif
