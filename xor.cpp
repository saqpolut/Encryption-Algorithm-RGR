#include "xor.h"
#include <cstring>
#include <random>
#include <chrono>

static const size_t KEY_SIZE = 32;
static const size_t STREAM_SIZE = 1024;

static void secure_memzero(void* ptr, size_t size) {
    volatile uint8_t* p = (volatile uint8_t*)ptr;
    for (size_t i = 0; i < size; ++i) {
        p[i] = 0;
    }
}

static void generate_keystream(const uint8_t* key, size_t key_len, 
                                uint8_t* output, size_t output_len) {
    for (size_t i = 0; i < output_len; ++i) {
        output[i] = key[i % key_len];
    }
}

extern "C" const AlgorithmInfo* get_algorithm_info() {
    static const AlgorithmInfo info = {
        .algorithm_name = "XOR",
        .key_size = KEY_SIZE
    };
    return &info;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type) {
    return input_size;
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size == 0 || input.data == nullptr || output->data == nullptr) {
        return 1;
    }
    
    if (output->size < input.size) {
        return 2;
    }
    
    uint8_t* keystream = new uint8_t[STREAM_SIZE];
    size_t processed = 0;
    
    while (processed < input.size) {
        size_t chunk_size = STREAM_SIZE;
        if (chunk_size > input.size - processed) {
            chunk_size = input.size - processed;
        }
        
        generate_keystream(key.data, key.size, keystream, chunk_size);
        
        for (size_t i = 0; i < chunk_size; ++i) {
            output->data[processed + i] = input.data[processed + i] ^ keystream[i];
        }
        
        processed += chunk_size;
    }
    
    output->size = input.size;
    
    secure_memzero(keystream, STREAM_SIZE);
    delete[] keystream;
    
    return 0;
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    return encrypt(key, input, output);
}
