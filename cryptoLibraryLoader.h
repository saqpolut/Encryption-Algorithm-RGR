#ifndef CRYPTOLIBRARYLOADER_H
#define CRYPTOLIBRARYLOADER_H

#include <string>

#ifdef _WIN32
#include <windows.h>
typedef HMODULE LibHandle;
#else
#include <dlfcn.h>
typedef void* LibHandle;
#endif

class CryptoLibraryLoader {
public:
    CryptoLibraryLoader();
    ~CryptoLibraryLoader();

    bool load(const std::string& libraryPath);
    void unload();
    bool isLoaded() const;

    std::string getAlgorithmName() const;
    int generateKey(char* outKey, int maxSize, int keyLength) const;
    int encrypt(const char* input, int inputLen, const char* key, char** output) const;
    int decrypt(const char* input, int inputLen, const char* key, char** output) const;
    void freeMemory(void* ptr) const;
    int validateKey(const char* key) const;

private:
    LibHandle handle;
    const char* (*pGetAlgorithmName)();
    int (*pGenerateKey)(char*, int, int);
    int (*pEncrypt)(const char*, int, const char*, char**);
    int (*pDecrypt)(const char*, int, const char*, char**);
    void (*pFreeMemory)(void*);
    int (*pValidateKey)(const char*);
};

#endif