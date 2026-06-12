// app/include/ICryptoAlgorithm.h
#ifndef ICRYPTOALGORITHM_H
#define ICRYPTOALGORITHM_H

#include <vector>
#include <cstdint>

using namespace std;

//макросы для экспорта или импорта в динамических библиотреках
#ifdef _WIN32
    #ifdef GRANDCHIFFRE_EXPORTS
        #define GRANDCHIFFRE_API __declspec(dllexport)
    #else
        #define GRANDCHIFFRE_API __declspec(dllimport)
    #endif
#else
    #define GRANDCHIFFRE_API
#endif

class ICryptoAlgorithm {
public:
    virtual ~ICryptoAlgorithm() = default;
    virtual void process(const vector<uint8_t>& key, vector<uint8_t>& data) = 0;  //шифрование/дишифрование
    virtual vector<uint8_t> generateKey(size_t length) = 0;  //генерация случайного ключа заданной длины
    //допустимая длина ключа 
    virtual size_t getMinKeyLength() const = 0;
    virtual size_t getMaxKeyLength() const = 0;
};

extern "C" {
    GRANDCHIFFRE_API ICryptoAlgorithm* create_algorithm();
    GRANDCHIFFRE_API void destroy_algorithm(ICryptoAlgorithm* alg);
}

#endif