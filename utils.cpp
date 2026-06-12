#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

void clearInputBuffer() {
    	int c;
    	while ((c = getchar()) != '\n' && c != EOF);
}

void removeNewline(char* str) {
    	if (str == 0) return;
    	int len = 0;
    	while (str[len] != '\0') {
        	len = len + 1;
    	}
    	if (len > 0 && str[len-1] == '\n') {
        	str[len-1] = '\0';
    	}
}

int isEmpty(const char* str) {
    	if (str == 0) return 1;
    	if (str[0] == '\0') return 1;
    	return 0;
}

void printSeparator() {
    	printf("\n========================================\n");
}

void waitForEnter() {
    	printf("\nНажмите Enter для продолжения...");
    	clearInputBuffer();
    	getchar();
}

int strLength(const char* str) {
    	int len = 0;
    	while (str[len] != '\0') {
        	len = len + 1;
    	}
    	return len;
}

void generateRandomKey(char* key, int maxLen) {
    	static int seeded = 0;
    	if (seeded == 0) {
        	srand(time(0));
        	seeded = 1;
    	}
    
    	int length = 8;
    	int r = rand() % 16;
    	length = length + r;
    
    	if (length > maxLen - 1) {
        	length = maxLen - 1;
    	}
    
    	int i = 0;
    	while (i < length) {
        	int r2 = rand() % 94;
        	key[i] = 33 + r2;
        	i = i + 1;
    	}
    	key[length] = '\0';
}
