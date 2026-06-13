#include "KeyValidator_chiffre.h"

bool KeyValidator::isValid(const std::vector<uint8_t>& key, size_t minLen, size_t maxLen) {
    return key.size() >= minLen && key.size() <= maxLen;
}

std::string KeyValidator::getErrorMessage() {
    return "Key validation failed: length out of allowed range";
}