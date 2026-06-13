#ifndef KEYVALIDATOR_CHIFFRE_H
#define KEYVALIDATOR_CHIFFRE_H

#include <vector>
#include <cstdint>
#include <string>

class KeyValidator {
public:
    static bool isValid(const std::vector<uint8_t>& key, size_t minLen, size_t maxLen);
    static std::string getErrorMessage();
};

#endif