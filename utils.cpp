#include "utils.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void removeNewline(char* str) {
    if (str == nullptr) return;
    int len = 0;
    while (str[len] != '\0') {
        len = len + 1;
    }
    if (len > 0 && str[len-1] == '\n') {
        str[len-1] = '\0';
    }
}

int isEmpty(const char* str) {
    if (str == nullptr) return 1;
    if (str[0] == '\0') return 1;
    return 0;
}

void printSeparator() {
    cout << "\n========================================" << endl;
}

void waitForEnter() {
    cout << "\nНажмите Enter для продолжения...";
    clearInputBuffer();
    cin.get();
}

int strLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len = len + 1;
    }
    return len;
}

#ifdef __cplusplus
}
#endif
