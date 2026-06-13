#include "skitala_impl.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>

void skitalaEncrypt(const char* input, char* output, int columns) {
    int len = strLength(input);
    if (len == 0 || columns < 2) {
        strcpy(output, input);
        return;
    }
    
    int rows = (len + columns - 1) / columns;
    char* matrix = (char*)malloc(rows * columns);
    if (!matrix) {
        strcpy(output, input);
        return;
    }
    
    memset(matrix, 0, rows * columns);
    
    int pos = 0;
    for (int col = 0; col < columns; col++) {
        for (int row = 0; row < rows; row++) {
            if (pos < len) {
                matrix[row * columns + col] = input[pos++];
            }
        }
    }
    
    pos = 0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            char ch = matrix[row * columns + col];
            if (ch != '\0') output[pos++] = ch;
        }
    }
    output[pos] = '\0';
    
    free(matrix);
}

void skitalaDecrypt(const char* input, char* output, int columns) {
    int len = strLength(input);
    if (len == 0 || columns < 2) {
        strcpy(output, input);
        return;
    }
    
    int rows = (len + columns - 1) / columns;
    char* matrix = (char*)malloc(rows * columns);
    if (!matrix) {
        strcpy(output, input);
        return;
    }
    
    memset(matrix, 0, rows * columns);
    
    int pos = 0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            if (pos < len) {
                matrix[row * columns + col] = input[pos++];
            }
        }
    }
    
    pos = 0;
    for (int col = 0; col < columns; col++) {
        for (int row = 0; row < rows; row++) {
            char ch = matrix[row * columns + col];
            if (ch != '\0') output[pos++] = ch;
        }
    }
    output[pos] = '\0';
    
    free(matrix);
}
