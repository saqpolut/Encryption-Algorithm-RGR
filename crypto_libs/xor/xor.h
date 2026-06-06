#ifndef XOR_H
#define XOR_H

#include <cstdint>
#include <cstddef>

extern "C" {

struct ConstBuffer {
    const uint8_t* data;
    size_t size;
};

struct MutBuffer {
    uint8_t* data;
    size_t size;
};

struct AlgorithmInfo {
    const char* algorithm_name;
    size_t key_size;
};

const AlgorithmInfo* get_algorithm_info();

size_t get_output_size(size_t input_size, int operation_type);

int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output);

int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output);

}

#endif
