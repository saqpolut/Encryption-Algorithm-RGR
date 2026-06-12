// app/include/TextCrypto.h
#ifndef TEXTCRYPTO_H
#define TEXTCRYPTO_H

#include "ICryptoAlgorithm.h"
#include <string>
#include <vector>

using namespace std;

class TextCrypto {
public:
//применение алгоритма шифровки/дишифровки к строке, возвращение резутьтата (стороку)
    static string encryptDecrypt(ICryptoAlgorithm* alg, const vector<uint8_t>& key, const string& input);
};

#endif