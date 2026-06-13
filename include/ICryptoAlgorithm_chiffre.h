#ifndef ICRYPTOALGORITHM_CHIFFRE_H
#define ICRYPTOALGORITHM_CHIFFRE_H

#include <vector>
#include <cstdint>
#include <cstddef>

// Макросы для экспорта/импорта в динамических библиотеках
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

    // Шифрование/дешифрование (in-place)
    virtual void process(const std::vector<uint8_t>& key, std::vector<uint8_t>& data) = 0;

    // Генерация случайного ключа заданной длины (байт)
    virtual std::vector<uint8_t> generateKey(size_t length) = 0;

    // Допустимые границы длины ключа
    virtual size_t getMinKeyLength() const = 0;
    virtual size_t getMaxKeyLength() const = 0;
};

extern "C" {
    GRANDCHIFFRE_API ICryptoAlgorithm* create_algorithm();
    GRANDCHIFFRE_API void destroy_algorithm(ICryptoAlgorithm* alg);
}

#endif
