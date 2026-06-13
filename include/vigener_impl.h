#ifndef VIGENER_IMPL_H
#define VIGENER_IMPL_H

#include <stddef.h>

void vigenerEncrypt(const char* input, char* output, const char* key);
void vigenerDecrypt(const char* input, char* output, const char* key);
void vigenerEncryptBlock(const char* input, char* output, size_t size, const char* key);
void vigenerDecryptBlock(const char* input, char* output, size_t size, const char* key);

#endif
