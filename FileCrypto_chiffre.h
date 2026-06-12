// app/include/FileCrypto.h
#ifndef FILECRYPTO_H
#define FILECRYPTO_H

#include "ICryptoAlgorithm.h"
#include <string>

using namespace std;

class FileCrypto {
public:
//чтение входного файла блоками, применение алгоритма и запись результата
    static bool encryptDecryptFile(ICryptoAlgorithm* alg, const vector<uint8_t>& key, const string& inputPath, const string& outputPath);
};

#endif