#include "TextCrypto_chiffre.h"
#include <stdexcept>

std::string TextCrypto::encryptDecrypt(ICryptoAlgorithm* alg,
                                       const std::vector<uint8_t>& key,
                                       const std::string& input) {
    if (!alg) throw std::invalid_argument("Algorithm is null");
    std::vector<uint8_t> data(input.begin(), input.end());
    alg->process(key, data);
    return std::string(data.begin(), data.end());
}