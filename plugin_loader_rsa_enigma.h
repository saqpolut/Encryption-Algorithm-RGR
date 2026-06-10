#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <cstdint>
#include <cstddef>

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE LibHandle;
    #define LOAD_LIB(name) LoadLibraryA(name)
    #define GET_SYM(lib, name) GetProcAddress(lib, name)
    #define FREE_LIB(lib) FreeLibrary(lib)
#else
    #include <dlfcn.h>
    typedef void* LibHandle;
    #define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
    #define GET_SYM(lib, name) dlsym(lib, name)
    #define FREE_LIB(lib) dlclose(lib)
#endif

typedef const char* (*GetInfoFunc)();
typedef void (*GenKeyFunc)(char*, size_t);
typedef void (*ProcessFunc)(const uint32_t*, size_t, const char*, uint32_t*);

struct Plugin {
    LibHandle handle = nullptr;
    GetInfoFunc get_name = nullptr;
    GetInfoFunc get_format = nullptr;
    GenKeyFunc gen_key = nullptr;
    ProcessFunc process = nullptr;
};

bool load_plugin(const char* filename, Plugin& p);
void free_plugin(Plugin& p);

#endif