// app/src/FileCrypto.cpp
#include "FileCrypto.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

bool FileCrypto::encryptDecryptFile(ICryptoAlgorithm* alg, const vector<uint8_t>& key, const string& inputPath, const string& outputPath) {
    if (!alg) return false;

    ifstream inFile(inputPath, ios::binary);  //открытие файла в бинарном режиме
    if (!inFile) {
        cerr << "Cannot open input file: " << inputPath << endl;
        return false;
    }
    ofstream outFile(outputPath, ios::binary);  //выходной файл в бинарном режиме
    if (!outFile) {
        cerr << "Cannot create output file: " << outputPath << endl;
        return false;
    }

    const size_t bufferSize = 64 * 1024;  //буфер для чтения блоками
    vector<uint8_t> buffer(bufferSize);

    //чтение файла порциями
    while (inFile) {
        inFile.read(reinterpret_cast<char*>(buffer.data()), bufferSize);
        streamsize bytesRead = inFile.gcount();  //сколько бит прочитанно
        if (bytesRead > 0) {
            vector<uint8_t> chunk(buffer.begin(), buffer.begin() + bytesRead);  //копируем прочитанное
            alg->process(key, chunk);  //шифровка/дишифровка
            outFile.write(reinterpret_cast<char*>(chunk.data()), chunk.size());  //запись обработанного блока в выходной файл
            if (!outFile) {
                cerr << "Write error" << endl;
                return false;
            }
        }
    }
    return true;
}