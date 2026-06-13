#include "rsa_key.h"
#include <cstdint>
#include <random>
#include <cstdio>
#include <cinttypes>

namespace rsa_keygen {

static uint64_t compute_gcd(uint64_t a, uint64_t b) {
    while (b != 0){
         uint64_t t = b; 
         b = a % b; 
         a = t;
        }
    return a;
}

static int64_t compute_mod_inverse(int64_t e, int64_t m) {
    int64_t m0 = m, t, q, x0 = 0, x1 = 1;
    if (m == 1) return 0;
    while (e > 1) {
        q = e / m; 
        t = m; 
        m = e % m; 
        e = t; 
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }
    return x1 < 0 ? x1 + m0 : x1;
}

static bool is_prime(uint64_t n) {
    if (n <= 3) return n > 1;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (uint64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

static uint64_t generate_prime(uint64_t min_val, uint64_t max_val) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(min_val, max_val);
    uint64_t num = dis(gen);
    while (!is_prime(num)) num = dis(gen);
    return num;
}

void generate(char* out_key, size_t max_len) {
    uint64_t p = generate_prime(300, 1000);
    uint64_t q = generate_prime(300, 1000);
    while (p == q) {
        q = generate_prime(300, 1000);
    }
    uint64_t n = p * q;
    uint64_t phi = (p - 1) * (q - 1);
    
    uint64_t e = 3;
    while (compute_gcd(e, phi) != 1) {
        e += 2;
    }
    
    uint64_t d = compute_mod_inverse(e, phi);
    std::snprintf(out_key, max_len, "%" PRIu64 ",%" PRIu64 ",%" PRIu64, n, e, d);
}

} 