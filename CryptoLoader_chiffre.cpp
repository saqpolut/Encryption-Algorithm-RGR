// app/src/CryptoLoader_chiffre.cpp
#include "CryptoLoader_chiffre.h"
#include <iostream>

using namespace std;

#ifdef _WIN32
#include <windows.h>
static HMODULE g_handle = nullptr;
#else
#include <dlfcn.h>
static void* g_handle = nullptr;
#endif

static ICryptoAlgorithm* g_algorithm = nullptr;
static void (*g_destroyFunc)(ICryptoAlgorithm*) = nullptr;

bool load_plugin(const string& libPath) {
    unload_plugin();

#ifdef _WIN32
    g_handle = LoadLibraryA(libPath.c_str());
    if (!g_handle) {
        cerr << "LoadLibrary failed: " << GetLastError() << endl;
        return false;
    }
    auto createFunc = (ICryptoAlgorithm* (*)()) GetProcAddress(g_handle, "create_algorithm");
    g_destroyFunc = (void (*)(ICryptoAlgorithm*)) GetProcAddress(g_handle, "destroy_algorithm");
#else
    g_handle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!g_handle) {
        cerr << "dlopen failed: " << dlerror() << endl;
        return false;
    }
    auto createFunc = (ICryptoAlgorithm* (*)()) dlsym(g_handle, "create_algorithm");
    g_destroyFunc = (void (*)(ICryptoAlgorithm*)) dlsym(g_handle, "destroy_algorithm");
#endif

    if (!createFunc || !g_destroyFunc) {
        cerr << "Missing create/destroy functions in plugin" << endl;
        unload_plugin();
        return false;
    }

    g_algorithm = createFunc();
    if (!g_algorithm) {
        cerr << "Failed to instantiate algorithm" << endl;
        unload_plugin();
        return false;
    }
    return true;
}

void unload_plugin() {
    if (g_algorithm && g_destroyFunc) {
        g_destroyFunc(g_algorithm);
        g_algorithm = nullptr;
    }
    if (g_handle) {
#ifdef _WIN32
        FreeLibrary(g_handle);
#else
        dlclose(g_handle);
#endif
        g_handle = nullptr;
    }
    g_destroyFunc = nullptr;
}

ICryptoAlgorithm* get_algorithm() {
    return g_algorithm;
}

bool is_plugin_loaded() {
    return g_algorithm != nullptr;
}