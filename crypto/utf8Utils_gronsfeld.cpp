#include "utf8Utils_gronsfeld.h"
#include <stdexcept>

namespace utf8 {

uint32_t decode(const std::string& str, size_t& pos) {
    if (pos >= str.size()) throw std::out_of_range("End of string");
    unsigned char ch = str[pos];
    if (ch < 0x80) {
        pos++;
        return ch;
    } else if ((ch & 0xE0) == 0xC0) {
        if (pos + 1 >= str.size()) throw std::runtime_error("Invalid UTF-8: incomplete 2-byte sequence");
        unsigned char c2 = str[pos+1];
        if ((c2 & 0xC0) != 0x80) throw std::runtime_error("Invalid UTF-8: invalid continuation byte");
        uint32_t cp = ((ch & 0x1F) << 6) | (c2 & 0x3F);
        if (cp < 0x80) throw std::runtime_error("Invalid UTF-8: overlong sequence");
        pos += 2;
        return cp;
    } else if ((ch & 0xF0) == 0xE0) {
        if (pos + 2 >= str.size()) throw std::runtime_error("Invalid UTF-8: incomplete 3-byte sequence");
        unsigned char c2 = str[pos+1];
        unsigned char c3 = str[pos+2];
        if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80)
            throw std::runtime_error("Invalid UTF-8: invalid continuation byte");
        uint32_t cp = ((ch & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
        if (cp < 0x800) throw std::runtime_error("Invalid UTF-8: overlong sequence");
        if (cp >= 0xD800 && cp <= 0xDFFF) throw std::runtime_error("Invalid UTF-8: surrogate codepoint");
        pos += 3;
        return cp;
    } else if ((ch & 0xF8) == 0xF0) {
        if (pos + 3 >= str.size()) throw std::runtime_error("Invalid UTF-8: incomplete 4-byte sequence");
        unsigned char c2 = str[pos+1];
        unsigned char c3 = str[pos+2];
        unsigned char c4 = str[pos+3];
        if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80)
            throw std::runtime_error("Invalid UTF-8: invalid continuation byte");
        uint32_t cp = ((ch & 0x07) << 18) | ((c2 & 0x3F) << 12) | ((c3 & 0x3F) << 6) | (c4 & 0x3F);
        if (cp < 0x10000) throw std::runtime_error("Invalid UTF-8: overlong sequence");
        if (cp > 0x10FFFF) throw std::runtime_error("Invalid UTF-8: codepoint beyond U+10FFFF");
        pos += 4;
        return cp;
    } else {
        throw std::runtime_error("Invalid UTF-8: invalid leading byte");
    }
}

void encode(uint32_t cp, std::string& out) {
    if (cp > 0x10FFFF) throw std::runtime_error("Invalid codepoint (beyond U+10FFFF)");
    if (cp >= 0xD800 && cp <= 0xDFFF) throw std::runtime_error("Invalid codepoint (surrogate)");
    if (cp < 0x80) {
        out.push_back(static_cast<char>(cp));
    } else if (cp < 0x800) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x110000) {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        throw std::runtime_error("Invalid codepoint");
    }
}

std::vector<uint32_t> toCodepoints(const std::string& utf8) {
    std::vector<uint32_t> result;
    size_t pos = 0;
    while (pos < utf8.size()) {
        result.push_back(decode(utf8, pos));
    }
    return result;
}

std::string fromCodepoints(const std::vector<uint32_t>& codepoints) {
    std::string result;
    for (uint32_t cp : codepoints) {
        encode(cp, result);
    }
    return result;
}

bool isLetter(uint32_t cp) {
    return (cp >= 0x41 && cp <= 0x5A) ||
           (cp >= 0x61 && cp <= 0x7A) ||
           (cp >= 0x0410 && cp <= 0x042F) ||
           (cp >= 0x0430 && cp <= 0x044F);
}

uint32_t shiftLetter(uint32_t cp, int delta) {
    if (!isLetter(cp)) return cp;
    
    int base, count;
    if (cp >= 0x41 && cp <= 0x5A) {
        base = 0x41; count = 26;
    } else if (cp >= 0x61 && cp <= 0x7A) {
        base = 0x61; count = 26;
    } else if (cp >= 0x0410 && cp <= 0x042F) {
        base = 0x0410; count = 32;
    } else {
        base = 0x0430; count = 32;
    }
    
    int offset = cp - base;
    int newOffset = (offset + delta) % count;
    if (newOffset < 0) newOffset += count;
    return base + newOffset;
}

}