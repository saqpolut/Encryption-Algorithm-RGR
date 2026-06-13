#include "cryptoLibraryLoader_gronsfeld.h"
#include <iostream>

CryptoLibraryLoader::CryptoLibraryLoader() : handle(nullptr),
    pGetAlgorithmName(nullptr), pGenerateKey(nullptr), pEncrypt(nullptr),
    pDecrypt(nullptr), pFreeMemory(nullptr), pValidateKey(nullptr) {}

CryptoLibraryLoader::~CryptoLibraryLoader() { unload(); }

bool CryptoLibraryLoader::load(const std::string& libraryPath) {
    unload();
#ifdef _WIN32
    handle = LoadLibraryA(libraryPath.c_str());
    if (!handle) { std::cerr << "Failed to load library: " << libraryPath << std::endl; return false; }
    pGetAlgorithmName = (const char* (*)()) GetProcAddress(handle, "getAlgorithmName");
    pGenerateKey = (int (*)(char*, int, int)) GetProcAddress(handle, "generateKey");
    pEncrypt = (int (*)(const char*, int, const char*, char**)) GetProcAddress(handle, "encrypt");
    pDecrypt = (int (*)(const char*, int, const char*, char**)) GetProcAddress(handle, "decrypt");
    pFreeMemory = (void (*)(void*)) GetProcAddress(handle, "freeMemory");
    pValidateKey = (int (*)(const char*)) GetProcAddress(handle, "validateKey");
#else
    handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
    if (!handle) { std::cerr << "Failed to load library: " << dlerror() << std::endl; return false; }
    pGetAlgorithmName = (const char* (*)()) dlsym(handle, "getAlgorithmName");
    pGenerateKey = (int (*)(char*, int, int)) dlsym(handle, "generateKey");
    pEncrypt = (int (*)(const char*, int, const char*, char**)) dlsym(handle, "encrypt");
    pDecrypt = (int (*)(const char*, int, const char*, char**)) dlsym(handle, "decrypt");
    pFreeMemory = (void (*)(void*)) dlsym(handle, "freeMemory");
    pValidateKey = (int (*)(const char*)) dlsym(handle, "validateKey");
#endif
    if (!pGetAlgorithmName || !pGenerateKey || !pEncrypt || !pDecrypt || !pFreeMemory || !pValidateKey) {
        std::cerr << "Missing required symbols" << std::endl;
        unload();
        return false;
    }
    return true;
}

void CryptoLibraryLoader::unload() {
    if (handle) {
#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
        handle = nullptr;
        pGetAlgorithmName = nullptr;
        pGenerateKey = nullptr;
        pEncrypt = nullptr;
        pDecrypt = nullptr;
        pFreeMemory = nullptr;
        pValidateKey = nullptr;
    }
}

bool CryptoLibraryLoader::isLoaded() const { return handle != nullptr; }

std::string CryptoLibraryLoader::getAlgorithmName() const {
    return pGetAlgorithmName ? pGetAlgorithmName() : "";
}

int CryptoLibraryLoader::generateKey(char* outKey, int maxSize, int keyLength) const {
    return pGenerateKey ? pGenerateKey(outKey, maxSize, keyLength) : -1;
}

int CryptoLibraryLoader::encrypt(const char* input, int inputLen, const char* key, char** output) const {
    return pEncrypt ? pEncrypt(input, inputLen, key, output) : -1;
}

int CryptoLibraryLoader::decrypt(const char* input, int inputLen, const char* key, char** output) const {
    return pDecrypt ? pDecrypt(input, inputLen, key, output) : -1;
}

void CryptoLibraryLoader::freeMemory(void* ptr) const {
    if (pFreeMemory) pFreeMemory(ptr);
}

int CryptoLibraryLoader::validateKey(const char* key) const {
    return pValidateKey ? pValidateKey(key) : 0;
}