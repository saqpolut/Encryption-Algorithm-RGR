#include "vigener_impl.h"
#include "utils.h"
#include <cstring>

void vigenerEncrypt(const char* input, char* output, const char* key) {
    int len = strLength(input);
    int keyLen = strLength(key);
    
    if (keyLen == 0) {
        std::strcpy(output, input);
        return;
    }
    
    for (int i = 0; i < len; i++) {
        output[i] = input[i] + key[i % keyLen];
    }
    output[len] = '\0';
}

void vigenerDecrypt(const char* input, char* output, const char* key) {
    int len = strLength(input);
    int keyLen = strLength(key);
    
    if (keyLen == 0) {
        std::strcpy(output, input);
        return;
    }
    
    for (int i = 0; i < len; i++) {
        output[i] = input[i] - key[i % keyLen];
    }
    output[len] = '\0';
}

void vigenerEncryptBlock(const char* input, char* output, size_t size, const char* key) {
    int keyLen = strLength(key);
    for (size_t i = 0; i < size; i++) {
        output[i] = input[i] + key[i % keyLen];
    }
}

void vigenerDecryptBlock(const char* input, char* output, size_t size, const char* key) {
    int keyLen = strLength(key);
    for (size_t i = 0; i < size; i++) {
        output[i] = input[i] - key[i % keyLen];
    }
}
