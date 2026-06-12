#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define MAX_TEXT_LEN 4096
#define MAX_KEY_LEN 256
#define MAX_PATH_LEN 512
#define BUFFER_SIZE 1024

void clearInputBuffer();
void removeNewline(char* str);
int isEmpty(const char* str);
void printSeparator();
void waitForEnter();
int strLength(const char* str);

#endif
