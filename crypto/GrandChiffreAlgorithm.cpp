#define GRANDCHIFFRE_EXPORTS
#include "GrandChiffreAlgorithm.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <stdexcept>
#include <cstdint>

using namespace std;

void GrandChiffreAlgorithm::rc4_init(const vector<uint8_t>& key, vector<uint8_t>& S) {
    S.resize(256);
    for (int i = 0; i < 256; ++i)
        S[i] = i;

    int j = 0;
    for (int i = 0; i < 256; ++i) {
        j = (j + S[i] + key[i % key.size()]) & 0xFF;
        swap(S[i], S[j]);
    }
}

void GrandChiffreAlgorithm::rc4_crypt(const vector<uint8_t>& S, vector<uint8_t>& data) {
    vector<uint8_t> state = S;
    int i = 0, j = 0;
    for (size_t k = 0; k < data.size(); ++k) {
        i = (i + 1) & 0xFF;
        j = (j + state[i]) & 0xFF;
        swap(state[i], state[j]);
        uint8_t keystream = state[(state[i] + state[j]) & 0xFF];
        data[k] ^= keystream;
    }
}

void GrandChiffreAlgorithm::process(const vector<uint8_t>& key, vector<uint8_t>& data) {
    if (key.size() < getMinKeyLength() || key.size() > getMaxKeyLength())
        throw invalid_argument("Key length out of allowed range");

    vector<uint8_t> S;
    rc4_init(key, S);
    rc4_crypt(S, data);
}

vector<uint8_t> GrandChiffreAlgorithm::generateKey(size_t length) {
    if (length < getMinKeyLength()) length = getMinKeyLength();
    if (length > getMaxKeyLength()) length = getMaxKeyLength();

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint8_t> dist(0, 255);

    vector<uint8_t> key(length);
    for (size_t i = 0; i < length; ++i)
        key[i] = dist(gen);
    return key;
}

// ЭКСПОРТИРУЕМЫЕ ФУНКЦИИ
extern "C" {

GRANDCHIFFRE_API ICryptoAlgorithm* create_algorithm() {
    return new GrandChiffreAlgorithm();
}

GRANDCHIFFRE_API void destroy_algorithm(ICryptoAlgorithm* alg) {
    delete alg;
}

} // extern "C"
