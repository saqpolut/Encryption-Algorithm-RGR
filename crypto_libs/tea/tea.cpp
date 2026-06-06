#include "tea.h"
#include <cstring>
#include <random>
#include <chrono>

static const size_t BLOCK_SIZE = 8;
static const size_t KEY_SIZE = 16;
static const uint32_t DELTA = 0x9E3779B9;
static const size_t ROUNDS = 32;

static void tea_encrypt_block(const uint32_t* key, const uint8_t* input, uint8_t* output) {
    uint32_t v0 = *(uint32_t*)(input);
    uint32_t v1 = *(uint32_t*)(input + 4);
    uint32_t sum = 0;
    
    for (size_t i = 0; i < ROUNDS; ++i) {
        sum += DELTA;
        v0 += ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
        v1 += ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
    }
    
    *(uint32_t*)(output) = v0;
    *(uint32_t*)(output + 4) = v1;
}

static void tea_decrypt_block(const uint32_t* key, const uint8_t* input, uint8_t* output) {
    uint32_t v0 = *(uint32_t*)(input);
    uint32_t v1 = *(uint32_t*)(input + 4);
    uint32_t sum = DELTA * ROUNDS;
    
    for (size_t i = 0; i < ROUNDS; ++i) {
        v1 -= ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
        v0 -= ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
        sum -= DELTA;
    }
    
    *(uint32_t*)(output) = v0;
    *(uint32_t*)(output + 4) = v1;
}

static void apply_pkcs7_padding(uint8_t* data, size_t data_len, size_t block_size) {
    size_t padding_len = block_size - (data_len % block_size);
    if (padding_len == 0) padding_len = block_size;
    
    for (size_t i = data_len; i < data_len + padding_len; ++i) {
        data[i] = padding_len;
    }
}

static size_t remove_pkcs7_padding(uint8_t* data, size_t data_len) {
    if (data_len == 0) return 0;
    
    uint8_t padding_len = data[data_len - 1];
    if (padding_len > BLOCK_SIZE || padding_len > data_len) {
        return data_len;
    }
    
    for (size_t i = data_len - padding_len; i < data_len; ++i) {
        if (data[i] != padding_len) {
            return data_len;
        }
    }
    
    return data_len - padding_len;
}

static void secure_memzero(void* ptr, size_t size) {
    volatile uint8_t* p = (volatile uint8_t*)ptr;
    for (size_t i = 0; i < size; ++i) {
        p[i] = 0;
    }
}

extern "C" const AlgorithmInfo* get_algorithm_info() {
    static const AlgorithmInfo info = {
        .algorithm_name = "TEA",
        .key_size = KEY_SIZE
    };
    return &info;
}

extern "C" size_t get_output_size(size_t input_size, int operation_type) {
    if (operation_type == 0) {
        size_t blocks = (input_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
        return blocks * BLOCK_SIZE;
    } else {
        return input_size;
    }
}

extern "C" int encrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size != KEY_SIZE || input.data == nullptr || output->data == nullptr) {
        return 1;
    }
    
    size_t padded_len = ((input.size + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
    if (output->size < padded_len + BLOCK_SIZE) {
        return 2;
    }
    
    uint32_t key_words[4];
    memcpy(key_words, key.data, KEY_SIZE);
    
    uint8_t* padded_data = new uint8_t[padded_len + BLOCK_SIZE];
    memcpy(padded_data, input.data, input.size);
    apply_pkcs7_padding(padded_data, input.size, BLOCK_SIZE);
    
    size_t final_len = ((input.size + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
    if (input.size % BLOCK_SIZE != 0) {
        final_len += BLOCK_SIZE;
    } else {
        final_len = input.size + BLOCK_SIZE;
    }
    
    for (size_t i = 0; i < final_len; i += BLOCK_SIZE) {
        tea_encrypt_block(key_words, padded_data + i, output->data + i);
    }
    
    output->size = final_len;
    
    secure_memzero(key_words, sizeof(key_words));
    secure_memzero(padded_data, padded_len + BLOCK_SIZE);
    delete[] padded_data;
    
    return 0;
}

extern "C" int decrypt(ConstBuffer key, ConstBuffer input, MutBuffer* output) {
    if (key.size != KEY_SIZE || input.size % BLOCK_SIZE != 0 || output->data == nullptr) {
        return 1;
    }
    
    if (output->size < input.size) {
        return 2;
    }
    
    uint32_t key_words[4];
    memcpy(key_words, key.data, KEY_SIZE);
    
    for (size_t i = 0; i < input.size; i += BLOCK_SIZE) {
        tea_decrypt_block(key_words, input.data + i, output->data + i);
    }
    
    size_t original_len = remove_pkcs7_padding(output->data, input.size);
    output->size = original_len;
    
    secure_memzero(key_words, sizeof(key_words));
    
    return 0;
}
