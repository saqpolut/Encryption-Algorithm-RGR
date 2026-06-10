// plugins/grandchiffre/src/GrandChiffreAlgorithm.h
#ifndef GRANDCHIFFRE_ALGORITHM_H
#define GRANDCHIFFRE_ALGORITHM_H

#include "ICryptoAlgorithm.h"

using namespace std;

class GrandChiffreAlgorithm : public ICryptoAlgorithm {
public:
    void process(const vector<uint8_t>& key, vector<uint8_t>& data) override;
    vector<uint8_t> generateKey(size_t length) override;
    size_t getMinKeyLength() const override { return 5; }  //минимум по гост (условно)
    size_t getMaxKeyLength() const override { return 256; }  //максимум для RC4

private:
    void rc4_init(const vector<uint8_t>& key, vector<uint8_t>& S);
    void rc4_crypt(const vector<uint8_t>& S, vector<uint8_t>& data);
};

#endif