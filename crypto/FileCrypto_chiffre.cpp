#include "FileCrypto_chiffre.h"
#include <fstream>
#include <iostream>
#include <vector>

bool FileCrypto::encryptDecryptFile(ICryptoAlgorithm* alg,
                                    const std::vector<uint8_t>& key,
                                    const std::string& inputPath,
                                    const std::string& outputPath) {
    if (!alg) return false;

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile) {
        std::cerr << "Cannot open input file: " << inputPath << std::endl;
        return false;
    }
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Cannot create output file: " << outputPath << std::endl;
        return false;
    }

    const size_t bufferSize = 64 * 1024;
    std::vector<uint8_t> buffer(bufferSize);

    while (inFile) {
        inFile.read(reinterpret_cast<char*>(buffer.data()), bufferSize);
        std::streamsize bytesRead = inFile.gcount();
        if (bytesRead > 0) {
            std::vector<uint8_t> chunk(buffer.begin(), buffer.begin() + bytesRead);
            alg->process(key, chunk);
            outFile.write(reinterpret_cast<char*>(chunk.data()), chunk.size());
            if (!outFile) {
                std::cerr << "Write error" << std::endl;
                return false;
            }
        }
    }
    return true;
}