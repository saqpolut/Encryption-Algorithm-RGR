#ifndef UTILS_H
#define UTILS_H

#include <iostream>

#define MAX_TEXT_LEN 4096
#define MAX_KEY_LEN 256
#define MAX_PATH_LEN 512
#define BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

void clearInputBuffer();
void removeNewline(char* str);
int isEmpty(const char* str);
void printSeparator();
void waitForEnter();
int strLength(const char* str);

#ifdef __cplusplus
}
#endif

#endif
