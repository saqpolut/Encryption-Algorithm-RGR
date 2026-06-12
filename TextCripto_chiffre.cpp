// app/src/TextCrypto.cpp
#include "TextCrypto.h"
#include <stdexcept>

using namespace std;

string TextCrypto::encryptDecrypt(ICryptoAlgorithm* alg, const vector<uint8_t>& key, const string& input) {
    if (!alg) throw invalid_argument("Algorithm is null");
    vector<uint8_t> data(input.begin(), input.end());  //преобразование в вектор байт
    alg->process(key, data);
    return string(data.begin(), data.end());
}