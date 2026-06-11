// app/src/KeyValidator.cpp
#include "KeyValidator.h"

using namespace std;

//проверка длины ключа
bool KeyValidator::isValid(const vector<uint8_t>& key, size_t minLen, size_t maxLen) {
    return key.size() >= minLen && key.size() <= maxLen;
}

//сообщение об ошибке при провале
string KeyValidator::getErrorMessage() {
    return "Key validation failed: length out of allowed range";
}