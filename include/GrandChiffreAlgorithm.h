#ifndef GRANDCHIFFREALGORITHM_H
#define GRANDCHIFFREALGORITHM_H

#include "ICryptoAlgorithm_chiffre.h"
#include <cstddef>

class GrandChiffreAlgorithm : public ICryptoAlgorithm {
public:
    void process(const std::vector<uint8_t>& key, std::vector<uint8_t>& data) override;
    std::vector<uint8_t> generateKey(size_t length) override;
    size_t getMinKeyLength() const override { return 5; }
    size_t getMaxKeyLength() const override { return 256; }

private:
    void rc4_init(const std::vector<uint8_t>& key, std::vector<uint8_t>& S);
    void rc4_crypt(const std::vector<uint8_t>& S, std::vector<uint8_t>& data);
};

#endif
