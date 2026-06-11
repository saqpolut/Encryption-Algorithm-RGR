#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

void clearInputBuffer() {
    	int c;
    	while ((c = getchar()) != '\n' && c != EOF);
}

void removeNewline(char* str) {
    	if (!str) return;
    	size_t len = strlen(str);
    	if (len > 0 && str[len-1] == '\n') {
        	str[len-1] = '\0';
    	}
}

int isEmpty(const char* str) {
    	return str == NULL || str[0] == '\0';
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
    	while (str[len]) len++;
    	return len;
}

void generateRandomKey(char* key, int maxLen) {
    	static int seeded = 0;
    	if (!seeded) {
        	srand((unsigned int)time(NULL));
        	seeded = 1;
    	}
    
    	int length = 8 + (rand() % 16); // ключ от 8 до 24 символов
    
    	if (length > maxLen - 1) {
        	length = maxLen - 1;
    	}
    
    	for (int i = 0; i < length; i++) {
        	// Генерируем печатные символы от 33 до 126
        	key[i] = (char)(33 + (rand() % 94));
    	}
    	key[length] = '\0';
}
