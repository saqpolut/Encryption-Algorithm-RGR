#include "cipher_loader.h"
#include <stdio.h>
#include <dlfcn.h>

#define PLUGIN_COUNT 2

static LoadedPlugin plugins[MAX_PLUGINS];
static int pluginsLoaded = 0;

static const char* pluginPaths[] = {
    "./libvigenere.so",
    "./libskitala.so"
};

int loadAllPlugins() {
    pluginsLoaded = 0;
    
    for (int i = 0; i < PLUGIN_COUNT; i++) {
        printf("Загрузка плагина: %s\n", pluginPaths[i]);
        
        void* handle = dlopen(pluginPaths[i], RTLD_LAZY);
        if (!handle) {
            printf("  Ошибка: %s\n", dlerror());
            continue;
        }
        
        plugins[pluginsLoaded].handle = handle;
        plugins[pluginsLoaded].get_name = (const char* (*)()) dlsym(handle, "plugin_get_name");
        plugins[pluginsLoaded].get_key_hint = (const char* (*)()) dlsym(handle, "plugin_get_key_hint");
        plugins[pluginsLoaded].is_key_valid = (int (*)(const char*)) dlsym(handle, "plugin_is_key_valid");
        plugins[pluginsLoaded].encrypt_text = (void (*)(const char*, char*, const char*)) dlsym(handle, "plugin_encrypt_text");
        plugins[pluginsLoaded].decrypt_text = (void (*)(const char*, char*, const char*)) dlsym(handle, "plugin_decrypt_text");
        plugins[pluginsLoaded].encrypt_block = (void (*)(const char*, char*, size_t, const char*)) dlsym(handle, "plugin_encrypt_block");
        plugins[pluginsLoaded].decrypt_block = (void (*)(const char*, char*, size_t, const char*)) dlsym(handle, "plugin_decrypt_block");
        
        if (!plugins[pluginsLoaded].get_name) {
            printf("  Ошибка: отсутствуют функции в плагине\n");
            dlclose(handle);
            continue;
        }
        
        pluginsLoaded++;
        printf("  → Загружен: %s\n", plugins[pluginsLoaded-1].get_name());
    }
    
    printf("\nЗагружено плагинов: %d\n", pluginsLoaded);
    return pluginsLoaded;
}

int getPluginsCount() {
    return pluginsLoaded;
}

LoadedPlugin* getPlugin(int index) {
    if (index < 0 || index >= pluginsLoaded) return 0;
    return &plugins[index];
}

void unloadAllPlugins() {
    for (int i = 0; i < pluginsLoaded; i++) {
        if (plugins[i].handle) dlclose(plugins[i].handle);
    }
    pluginsLoaded = 0;
}
