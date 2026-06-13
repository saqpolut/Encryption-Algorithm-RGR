#ifndef TEXTCRYPTO_CHIFFRE_H
#define TEXTCRYPTO_CHIFFRE_H

#include "ICryptoAlgorithm_chiffre.h"
#include <string>
#include <vector>

class TextCrypto {
public:
    static std::string encryptDecrypt(ICryptoAlgorithm* alg,
                                      const std::vector<uint8_t>& key,
                                      const std::string& input);
};

#endif