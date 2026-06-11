#ifndef CRYPTOINTERFACE_H
#define CRYPTOINTERFACE_H

#include <cstdint>

#ifdef _WIN32
    #ifdef EXPORTING_DLL
        #define CRYPTO_API __declspec(dllexport)
    #else
        #define CRYPTO_API __declspec(dllimport)
    #endif
#else
    #define CRYPTO_API __attribute__((visibility("default")))
#endif

extern "C" {

    CRYPTO_API const char* getAlgorithmName();

    CRYPTO_API int generateKey(char* outKey, int maxSize, int keyLength);

    CRYPTO_API int encrypt(const char* input, int inputLen, const char* key, char** output);

    CRYPTO_API int decrypt(const char* input, int inputLen, const char* key, char** output);

    CRYPTO_API void freeMemory(void* ptr);

    CRYPTO_API int validateKey(const char* key);
}

#endif