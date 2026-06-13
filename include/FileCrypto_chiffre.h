#ifndef FILECRYPTO_CHIFFRE_H
#define FILECRYPTO_CHIFFRE_H

#include "ICryptoAlgorithm_chiffre.h"
#include <string>
#include <vector>

class FileCrypto {
public:
    static bool encryptDecryptFile(ICryptoAlgorithm* alg,
                                   const std::vector<uint8_t>& key,
                                   const std::string& inputPath,
                                   const std::string& outputPath);
};

#endif