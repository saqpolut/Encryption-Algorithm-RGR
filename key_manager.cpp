#include "key_manager.h"
#include <stdlib.h>
#include <time.h>

void generateKey(char* key, int maxLen, int isSkitala) {
    	static int seeded = 0;
    	if (!seeded) {
        	srand(time(0));
        	seeded = 1;
    	}
    
    	if (isSkitala) {
        	int cols = 2 + (rand() % 19);
        	if (cols < 10) {
            	key[0] = '0' + cols;
            	key[1] = '\0';
        	} else {
            		key[0] = '0' + (cols / 10);
            		key[1] = '0' + (cols % 10);
            		key[2] = '\0';
        	}
    	} else {
        	int length = 8 + (rand() % 16);
        	if (length > maxLen - 1) length = maxLen - 1;
        
        	for (int i = 0; i < length; i++) {
            		key[i] = 33 + (rand() % 94);
        	}
        	key[length] = '\0';
    	}
}
