#include "cipher_loader.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibHandle;
    #define DLOPEN(name) LoadLibraryA(name)
    #define DLSYM(handle, sym) (void*)GetProcAddress(handle, sym)
    #define DLCLOSE(handle) FreeLibrary(handle)
    #define DLERROR() "LoadLibrary failed"
#else
    #include <dlfcn.h>
    typedef void* LibHandle;
    #define DLOPEN(name) dlopen(name, RTLD_LAZY)
    #define DLSYM(handle, sym) dlsym(handle, sym)
    #define DLCLOSE(handle) dlclose(handle)
    #define DLERROR() dlerror()
#endif

using namespace std;

#define PLUGIN_COUNT 2

static LoadedPlugin plugins[MAX_PLUGINS];
static int pluginsLoaded = 0;

static const char* pluginPaths[] = {
    "./libvigener.so",
    "./libskitala.so"
};

template<typename T>
static void loadSymbol(LibHandle handle, T& funcPtr, const char* symbolName) {
    void* sym = DLSYM(handle, symbolName);
    if (sym) {
        memcpy(&funcPtr, &sym, sizeof(funcPtr));
    } else {
        funcPtr = nullptr;
    }
}

int loadAllPlugins() {
    pluginsLoaded = 0;
    
    for (int i = 0; i < PLUGIN_COUNT; i++) {
        cout << "Загрузка плагина: " << pluginPaths[i] << endl;
        
        LibHandle handle = DLOPEN(pluginPaths[i]);
        if (!handle) {
            cout << "  Ошибка: " << DLERROR() << endl;
            continue;
        }
        
        plugins[pluginsLoaded].handle = (void*)handle;
        
        loadSymbol(handle, plugins[pluginsLoaded].get_name, "plugin_get_name");
        loadSymbol(handle, plugins[pluginsLoaded].get_key_hint, "plugin_get_key_hint");
        loadSymbol(handle, plugins[pluginsLoaded].is_key_valid, "plugin_is_key_valid");
        loadSymbol(handle, plugins[pluginsLoaded].encrypt_text, "plugin_encrypt_text");
        loadSymbol(handle, plugins[pluginsLoaded].decrypt_text, "plugin_decrypt_text");
        loadSymbol(handle, plugins[pluginsLoaded].encrypt_block, "plugin_encrypt_block");
        loadSymbol(handle, plugins[pluginsLoaded].decrypt_block, "plugin_decrypt_block");
        
        if (!plugins[pluginsLoaded].get_name) {
            cout << "  Ошибка: отсутствуют функции в плагине" << endl;
            DLCLOSE(handle);
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
        if (plugins[i].handle) DLCLOSE((LibHandle)plugins[i].handle);
    }
    pluginsLoaded = 0;
}