#include "utf8Utils.h"
#include <stdexcept>

namespace utf8 {

uint32_t decode(const std::string& str, size_t& pos) {  //чтение одного символа
    if (pos >= str.size()) throw std::out_of_range("End of string");
    unsigned char ch = str[pos];
    if (ch < 0x80) {  //1 байт
        pos++;
        return ch;
    } else if ((ch & 0xE0) == 0xC0) {  //2 байта
        if (pos + 1 >= str.size()) throw std::runtime_error("Invalid UTF-8");
        uint32_t cp = ((ch & 0x1F) << 6) | (str[pos+1] & 0x3F);
        pos += 2;
        return cp;
    } else if ((ch & 0xF0) == 0xE0) {  //3 байта
        if (pos + 2 >= str.size()) throw std::runtime_error("Invalid UTF-8");
        uint32_t cp = ((ch & 0x0F) << 12) | ((str[pos+1] & 0x3F) << 6) | (str[pos+2] & 0x3F);
        pos += 3;
        return cp;
    } else if ((ch & 0xF8) == 0xF0) {  //4 байта
        if (pos + 3 >= str.size()) throw std::runtime_error("Invalid UTF-8");
        uint32_t cp = ((ch & 0x07) << 18) | ((str[pos+1] & 0x3F) << 12) | ((str[pos+2] & 0x3F) << 6) | (str[pos+3] & 0x3F);
        pos += 4;
        return cp;
    } else {
        throw std::runtime_error("Invalid UTF-8 sequence");
    }
}

void encode(uint32_t cp, std::string& out) {  //кодирование кодовой точки
    if (cp < 0x80) {  //1 байт
        out.push_back(static_cast<char>(cp));
    } else if (cp < 0x800) {  //2 байта
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {  //3 байта
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x110000) {  //4 байта
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        throw std::runtime_error("Invalid codepoint");
    }
}

std::vector<uint32_t> toCodepoints(const std::string& utf8) {  //разбиение строки на вектор кодовых точек
    std::vector<uint32_t> result;
    size_t pos = 0;
    while (pos < utf8.size()) {
        result.push_back(decode(utf8, pos));
    }
    return result;
}

std::string fromCodepoints(const std::vector<uint32_t>& codepoints) {  //сборка строки из вектора кодовых точек
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
    if (cp >= 0x41 && cp <= 0x5A) {  //верхний регистр латиницы
        base = 0x41; count = 26;
    } else if (cp >= 0x61 && cp <= 0x7A) {  //нижний регистр латиницы
        base = 0x61; count = 26;
    } else if (cp >= 0x0410 && cp <= 0x042F) {  //верхний регистр кириллицы
        base = 0x0410; count = 32;
    } else {  //нижний регистр кириллицы
        base = 0x0430; count = 32;
    }
    
    int offset = cp - base;
    int newOffset = (offset + delta) % count;
    if (newOffset < 0) newOffset += count;
    return base + newOffset;
}

}