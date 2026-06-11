#ifndef UTF8UTILS_H
#define UTF8UTILS_H

#include <string>
#include <vector>
#include <cstdint>

namespace utf8 {

    uint32_t decode(const std::string& str, size_t& pos);
    void encode(uint32_t codepoint, std::string& out);
    std::vector<uint32_t> toCodepoints(const std::string& utf8);
    std::string fromCodepoints(const std::vector<uint32_t>& codepoints);
    bool isLetter(uint32_t cp);
    uint32_t shiftLetter(uint32_t cp, int delta);
}

#endif