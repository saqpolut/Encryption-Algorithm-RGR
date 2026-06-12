#include "cipher_loader.h"
#include <iostream>
#include <dlfcn.h>

using namespace std;

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
        cout << "Загрузка плагина: " << pluginPaths[i] << endl;
        
        void* handle = dlopen(pluginPaths[i], RTLD_LAZY);
        if (!handle) {
            cout << "  Ошибка: " << dlerror() << endl;
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
            cout << "  Ошибка: отсутствуют функции в плагине" << endl;
            dlclose(handle);
            continue;
        }
        
        pluginsLoaded++;
        cout << "  → Загружен: " << plugins[pluginsLoaded-1].get_name() << endl;
    }
    
    cout << "\nЗагружено плагинов: " << pluginsLoaded << endl;
    return pluginsLoaded;
}

int getPluginsCount() {
    return pluginsLoaded;
}

LoadedPlugin* getPlugin(int index) {
    if (index < 0 || index >= pluginsLoaded) return nullptr;
    return &plugins[index];
}

void unloadAllPlugins() {
    for (int i = 0; i < pluginsLoaded; i++) {
        if (plugins[i].handle) dlclose(plugins[i].handle);
    }
    pluginsLoaded = 0;
}
