//#define EXPORTING_DLL

#include "cryptoInterface_gronsfeld.h"
#include "utf8Utils_gronsfeld.h"
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <random>

static bool isValidKey(const std::string& key) {
    if (key.empty() || key.length() > 256) return false;
    for (char c : key) if (c < '0' || c > '9') return false;
    return true;
}

static std::string process(const std::string& input, const std::string& key, int direction) {
    if (!isValidKey(key)) return "";
    
    try {
        std::vector<uint32_t> codepoints = utf8::toCodepoints(input);
        size_t keyLen = key.length(), keyPos = 0;
        
        for (uint32_t& cp : codepoints) {
            if (utf8::isLetter(cp)) {
                int shift = key[keyPos % keyLen] - '0';
                if (direction == -1) shift = -shift;
                cp = utf8::shiftLetter(cp, shift);
                keyPos++;
            }
        }
        return utf8::fromCodepoints(codepoints);
    } catch (const std::exception&) {
        return "";
    }
}

CRYPTO_API const char* getAlgorithmName() {
    return "Gronsfeld Cipher";
}

CRYPTO_API int generateKey(char* outKey, int maxSize, int keyLength) {
    if (!outKey || maxSize <= 0) return -1;
    if (keyLength <= 0) keyLength = 8;
    if (keyLength >= maxSize) return -1;
    
    static std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(0, 9);
    
    for (int i = 0; i < keyLength; ++i)
        outKey[i] = '0' + dist(rng);
    outKey[keyLength] = '\0';
    return keyLength;
}

CRYPTO_API int encrypt(const char* input, int inputLen, const char* key, char** output) {
    if (!input || inputLen <= 0 || !key || !output) return -1;
    std::string inStr(input, inputLen);
    std::string result = process(inStr, key, +1);
    if (result.empty()) return -1;
    *output = static_cast<char*>(malloc(result.size() + 1));
    if (!*output) return -1;
    memcpy(*output, result.c_str(), result.size() + 1);
    return static_cast<int>(result.size());
}

CRYPTO_API int decrypt(const char* input, int inputLen, const char* key, char** output) {
    if (!input || inputLen <= 0 || !key || !output) return -1;
    std::string inStr(input, inputLen);
    std::string result = process(inStr, key, -1);
    if (result.empty()) return -1;
    *output = static_cast<char*>(malloc(result.size() + 1));
    if (!*output) return -1;
    memcpy(*output, result.c_str(), result.size() + 1);
    return static_cast<int>(result.size());
}

CRYPTO_API void freeMemory(void* ptr) {
    if (ptr) free(ptr);
}

CRYPTO_API int validateKey(const char* key) {
    if (!key) return 0;
    return isValidKey(key) ? 1 : 0;
}