#include "plugin_api.h"
#include "enigma_key.h"
#include <array>
#include <cstring>
#include <stdexcept>

constexpr std::array<uint8_t, 26> rotor1 = {4, 10, 12, 5, 11, 6, 3, 16, 21, 25, 13, 19, 14, 22, 24, 7, 23, 20, 18, 15, 0, 8, 1, 17, 2, 9};
constexpr std::array<uint8_t, 26> rotor2 = {0, 9, 3, 10, 18, 8, 17, 20, 23, 1, 11, 7, 22, 19, 12, 2, 16, 6, 25, 13, 15, 24, 5, 21, 14, 4};
constexpr std::array<uint8_t, 26> rotor3 = {1, 3, 5, 7, 9, 11, 2, 15, 17, 19, 23, 21, 25, 13, 24, 4, 8, 22, 6, 0, 10, 12, 20, 18, 16, 14};
constexpr std::array<uint8_t, 26> reflector = {24, 17, 20, 7, 16, 18, 11, 3, 15, 23, 13, 6, 14, 10, 12, 8, 4, 1, 5, 25, 2, 22, 21, 9, 0, 19};

static std::array<uint8_t, 26> compute_inverse(const std::array<uint8_t, 26>& perm) {
    std::array<uint8_t, 26> inv{};
    for (uint8_t i = 0; i < 26; ++i) inv[perm[i]] = i;
    return inv;
}

static const std::array<uint8_t, 26> inv_rotor1 = compute_inverse(rotor1);
static const std::array<uint8_t, 26> inv_rotor2 = compute_inverse(rotor2);
static const std::array<uint8_t, 26> inv_rotor3 = compute_inverse(rotor3);

static void step_rotors(uint8_t& r, uint8_t& m, uint8_t& l) {
    r = (r + 1) % 26;
    if (r == 0) {
        m = (m + 1) % 26;
        if (m == 0) {
            l = (l + 1) % 26;
        }
    }
}

static uint8_t process_byte(uint8_t c, uint8_t& r, uint8_t& m, uint8_t& l) {
    if (c >= 'A' && c <= 'Z') {
        step_rotors(r, m, l);
        uint8_t pos = c - 'A';

        pos = (rotor3[(pos + r) % 26] - r + 26) % 26;
        pos = (rotor2[(pos + m) % 26] - m + 26) % 26;
        pos = (rotor1[(pos + l) % 26] - l + 26) % 26;
        
        pos = reflector[pos];

        pos = (inv_rotor1[(pos + l) % 26] - l + 26) % 26;
        pos = (inv_rotor2[(pos + m) % 26] - m + 26) % 26;
        pos = (inv_rotor3[(pos + r) % 26] - r + 26) % 26;
        
        return pos + 'A';
    }
    if (c >= 'a' && c <= 'z') {
        return process_byte(c - 'a' + 'A', r, m, l) - 'A' + 'a';
    }

    return c;
}

extern "C" {

PLUGIN_EXPORT const char* plugin_get_name() { return "Enigma"; }
PLUGIN_EXPORT const char* plugin_get_key_format() { return "3 буквы латинского алфавита (A-Z)"; }

PLUGIN_EXPORT void plugin_generate_key(char* out_key, size_t max_len) {
    enigma_keygen::generate(out_key, max_len);
}

PLUGIN_EXPORT void plugin_process_data(const uint32_t* input, size_t len, const char* key, uint32_t* output) {
    if (key == nullptr || std::strlen(key) < 3) {
        throw std::runtime_error("Ошибка: Ключ Enigma должен содержать минимум 3 символа.");
    }
    
    uint8_t r = (key[0] >= 'A' && key[0] <= 'Z') ? (key[0] - 'A') : 0;
    uint8_t m = (key[1] >= 'A' && key[1] <= 'Z') ? (key[1] - 'A') : 0;
    uint8_t l = (key[2] >= 'A' && key[2] <= 'Z') ? (key[2] - 'A') : 0;

    for (size_t i = 0; i < len; ++i) {
        uint8_t byte_in = static_cast<uint8_t>(input[i] & 0xFF);
        uint8_t byte_out = process_byte(byte_in, r, m, l);
        output[i] = static_cast<uint32_t>(byte_out);
    }
}

}
