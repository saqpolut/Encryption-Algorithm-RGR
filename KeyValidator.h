// app/include/KeyValidator.h
#ifndef KEYVALIDATOR_H
#define KEYVALIDATOR_H

#include <vector>
#include <cstdint>
#include <string>

using namespace std;

class KeyValidator {
public:
    static bool isValid(const vector<uint8_t>& key, size_t minLen, size_t maxLen);  //проверка длины ключа
    static string getErrorMessage();  //сообщение об ошибке при провале
};

#endif