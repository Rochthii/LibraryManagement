/**
 * @file VietnameseUtils.cpp
 * @brief Implementation of Vietnamese text processing utilities
 * @author Library Management Project Team  
 * @version 2.1.0
 * @date 2025-10-01
 * 
 * @note Unicode Mapping Sources:
 * All Vietnamese Unicode character mappings are derived from official sources:
 * - Unicode Consortium: https://unicode.org/charts/PDF/U0000.pdf
 * - Case Folding Data: https://unicode.org/Public/UCD/latest/ucd/CaseFolding.txt
 * - Latin-1 Supplement: U+0080–U+00FF (à, á, â, etc.)
 * - Latin Extended-A: U+0100–U+017F (ă, đ, etc.) 
 * - Latin Extended Additional: U+1E00–U+1EFF (ạ, ả, ậ, etc.)
 * 
 * Implementation follows UTF-8 specification (RFC 3629) and C++17 standards.
 * 
 * @copyright Unicode data © Unicode, Inc. Licensed under Unicode License Agreement
 */

/**
 * @file VietnameseUtils.cpp
 * @brief Xử lý chuỗi tiếng Việt (Unicode) cho chương trình thư viện
 * @version 2.2.0
 * @date 2025-10-10
 * 
 * - Chuẩn hóa chuỗi (tên sách, tác giả, thể loại): "NGUYỄN VĂN AN" → "Nguyễn Văn An".
 * - Bỏ dấu tiếng Việt cho tìm kiếm: "Nguyễn" → "Nguyen".
 * - Không dùng vector, dùng danh sách liên kết đơn tự cài đặt.
 * - Xử lý UTF-8 đơn giản, chỉ giữ các hàm cần thiết.
 */

#include "../include/VietnameseUtils.h"
#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <cctype>
#include <unordered_map>

using namespace std;

namespace VietnameseUtils {

// Bảng tra cứu cho tiếng Việt
namespace {
    // Kiểm tra ký tự khoảng trắng
    constexpr bool isWhitespace(char c) noexcept { 
        return c == ' ' || c == '\t' || c == '\n' || c == '\r'; 
    }

    // Kiểm tra chữ cái ASCII
    constexpr bool isAsciiLetter(char c) noexcept { 
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); 
    }

    // Kiểm tra số ASCII
    constexpr bool isAsciiDigit(char c) noexcept { 
        return c >= '0' && c <= '9'; 
    }

    // Bảng chuyển chữ thường thành HOA
    const unordered_map<uint32_t, uint32_t> LOWER_TO_UPPER = {
        // ASCII
        {'a', 'A'}, {'b', 'B'}, {'c', 'C'}, {'d', 'D'}, {'e', 'E'}, {'f', 'F'},
        {'g', 'G'}, {'h', 'H'}, {'i', 'I'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'},
        {'m', 'M'}, {'n', 'N'}, {'o', 'O'}, {'p', 'P'}, {'q', 'Q'}, {'r', 'R'},
        {'s', 'S'}, {'t', 'T'}, {'u', 'U'}, {'v', 'V'}, {'w', 'W'}, {'x', 'X'},
        {'y', 'Y'}, {'z', 'Z'},
        // Tiếng Việt
        {0x00E0, 0x00C0}, {0x00E1, 0x00C1}, {0x1EA3, 0x1EA2}, {0x00E3, 0x00C3}, {0x1EA1, 0x1EA0}, // a, à, á, ả, ã, ạ
        {0x0103, 0x0102}, {0x1EB1, 0x1EB0}, {0x1EAF, 0x1EAE}, {0x1EB3, 0x1EB2}, {0x1EB5, 0x1EB4}, {0x1EB7, 0x1EB6}, // ă
        {0x00E2, 0x00C2}, {0x1EA7, 0x1EA6}, {0x1EA5, 0x1EA4}, {0x1EA9, 0x1EA8}, {0x1EAB, 0x1EAA}, {0x1EAD, 0x1EAC}, // â
        {0x0111, 0x0110}, // đ
        {0x00E8, 0x00C8}, {0x00E9, 0x00C9}, {0x1EBB, 0x1EBA}, {0x1EBD, 0x1EBC}, {0x1EB9, 0x1EB8}, // e, è, é, ẻ, ẽ, ẹ
        {0x00EA, 0x00CA}, {0x1EC1, 0x1EC0}, {0x1EBF, 0x1EBE}, {0x1EC3, 0x1EC2}, {0x1EC5, 0x1EC4}, {0x1EC7, 0x1EC6}, // ê
        {0x00EC, 0x00CC}, {0x00ED, 0x00CD}, {0x1EC9, 0x1EC8}, {0x0129, 0x0128}, {0x1ECB, 0x1ECA}, // i
        {0x00F2, 0x00D2}, {0x00F3, 0x00D3}, {0x1ECF, 0x1ECE}, {0x00F5, 0x00D5}, {0x1ECD, 0x1ECC}, // o, ò, ó, ỏ, õ, ọ
        {0x00F4, 0x00D4}, {0x1ED3, 0x1ED2}, {0x1ED1, 0x1ED0}, {0x1ED5, 0x1ED4}, {0x1ED7, 0x1ED6}, {0x1ED9, 0x1ED8}, // ô
        {0x01A1, 0x01A0}, {0x1EDD, 0x1EDC}, {0x1EDB, 0x1EDA}, {0x1EDF, 0x1EDE}, {0x1EE1, 0x1EE0}, {0x1EE3, 0x1EE2}, // ơ
        {0x00F9, 0x00D9}, {0x00FA, 0x00DA}, {0x1EE7, 0x1EE6}, {0x0169, 0x0168}, {0x1EE5, 0x1EE4}, // u, ù, ú, ủ, ũ, ụ
        {0x01B0, 0x01AF}, {0x1EEB, 0x1EEA}, {0x1EE9, 0x1EE8}, {0x1EED, 0x1EEC}, {0x1EEF, 0x1EEE}, {0x1EF1, 0x1EF0}, // ư
        {0x1EF3, 0x1EF2}, {0x00FD, 0x00DD}, {0x1EF7, 0x1EF6}, {0x1EF9, 0x1EF8}, {0x1EF5, 0x1EF4} // y
    };

    // Bảng chuyển chữ HOA thành thường
    const unordered_map<uint32_t, uint32_t> UPPER_TO_LOWER = {
        // ASCII
        {'A', 'a'}, {'B', 'b'}, {'C', 'c'}, {'D', 'd'}, {'E', 'e'}, {'F', 'f'},
        {'G', 'g'}, {'H', 'h'}, {'I', 'i'}, {'J', 'j'}, {'K', 'k'}, {'L', 'l'},
        {'M', 'm'}, {'N', 'n'}, {'O', 'o'}, {'P', 'p'}, {'Q', 'q'}, {'R', 'r'},
        {'S', 's'}, {'T', 't'}, {'U', 'u'}, {'V', 'v'}, {'W', 'w'}, {'X', 'x'},
        {'Y', 'y'}, {'Z', 'z'},
        // Tiếng Việt
        {0x00C0, 0x00E0}, {0x00C1, 0x00E1}, {0x1EA2, 0x1EA3}, {0x00C3, 0x00E3}, {0x1EA0, 0x1EA1}, // A, À, Á, Ả, Ã, Ạ
        {0x0102, 0x0103}, {0x1EB0, 0x1EB1}, {0x1EAE, 0x1EAF}, {0x1EB2, 0x1EB3}, {0x1EB4, 0x1EB5}, {0x1EB6, 0x1EB7}, // Ă
        {0x00C2, 0x00E2}, {0x1EA6, 0x1EA7}, {0x1EA4, 0x1EA5}, {0x1EA8, 0x1EA9}, {0x1EAA, 0x1EAB}, {0x1EAC, 0x1EAD}, // Â
        {0x0110, 0x0111}, // Đ
        {0x00C8, 0x00E8}, {0x00C9, 0x00E9}, {0x1EBA, 0x1EBB}, {0x1EBC, 0x1EBD}, {0x1EB8, 0x1EB9}, // E, È, É, Ẻ, Ẽ, Ẹ
        {0x00CA, 0x00EA}, {0x1EC0, 0x1EC1}, {0x1EBE, 0x1EBF}, {0x1EC2, 0x1EC3}, {0x1EC4, 0x1EC5}, {0x1EC6, 0x1EC7}, // Ê
        {0x00CC, 0x00EC}, {0x00CD, 0x00ED}, {0x1EC8, 0x1EC9}, {0x0128, 0x0129}, {0x1ECA, 0x1ECB}, // I
        {0x00D2, 0x00F2}, {0x00D3, 0x00F3}, {0x1ECE, 0x1ECF}, {0x00D5, 0x00F5}, {0x1ECC, 0x1ECD}, // O, Ò, Ó, Ỏ, Õ, Ọ
        {0x00D4, 0x00F4}, {0x1ED2, 0x1ED3}, {0x1ED0, 0x1ED1}, {0x1ED4, 0x1ED5}, {0x1ED6, 0x1ED7}, {0x1ED8, 0x1ED9}, // Ô
        {0x01A0, 0x01A1}, {0x1EDC, 0x1EDD}, {0x1EDA, 0x1EDB}, {0x1EDE, 0x1EDF}, {0x1EE0, 0x1EE1}, {0x1EE2, 0x1EE3}, // Ơ
        {0x00D9, 0x00F9}, {0x00DA, 0x00FA}, {0x1EE6, 0x1EE7}, {0x0168, 0x0169}, {0x1EE4, 0x1EE5}, // U, Ù, Ú, Ủ, Ũ, Ụ
        {0x01AF, 0x01B0}, {0x1EEA, 0x1EEB}, {0x1EE8, 0x1EE9}, {0x1EEC, 0x1EED}, {0x1EEE, 0x1EEF}, {0x1EF0, 0x1EF1}, // Ư
        {0x1EF2, 0x1EF3}, {0x00DD, 0x00FD}, {0x1EF6, 0x1EF7}, {0x1EF8, 0x1EF9}, {0x1EF4, 0x1EF5} // Y
    };

    // Bảng bỏ dấu tiếng Việt (cho tìm kiếm)
    const unordered_map<uint32_t, char> ACCENT_MAP = {
        {0x0061, 'a'}, {0x00E0, 'a'}, {0x00E1, 'a'}, {0x1EA3, 'a'}, {0x00E3, 'a'}, {0x1EA1, 'a'}, // a
        {0x0103, 'a'}, {0x1EB1, 'a'}, {0x1EAF, 'a'}, {0x1EB3, 'a'}, {0x1EB5, 'a'}, {0x1EB7, 'a'}, // ă
        {0x00E2, 'a'}, {0x1EA7, 'a'}, {0x1EA5, 'a'}, {0x1EA9, 'a'}, {0x1EAB, 'a'}, {0x1EAD, 'a'}, // â
        {0x0065, 'e'}, {0x00E8, 'e'}, {0x00E9, 'e'}, {0x1EBB, 'e'}, {0x1EBD, 'e'}, {0x1EB9, 'e'}, // e
        {0x00EA, 'e'}, {0x1EC1, 'e'}, {0x1EBF, 'e'}, {0x1EC3, 'e'}, {0x1EC5, 'e'}, {0x1EC7, 'e'}, // ê
        {0x0069, 'i'}, {0x00EC, 'i'}, {0x00ED, 'i'}, {0x1EC9, 'i'}, {0x0129, 'i'}, {0x1ECB, 'i'}, // i
        {0x006F, 'o'}, {0x00F2, 'o'}, {0x00F3, 'o'}, {0x1ECF, 'o'}, {0x00F5, 'o'}, {0x1ECD, 'o'}, // o
        {0x00F4, 'o'}, {0x1ED3, 'o'}, {0x1ED1, 'o'}, {0x1ED5, 'o'}, {0x1ED7, 'o'}, {0x1ED9, 'o'}, // ô
        {0x01A1, 'o'}, {0x1EDD, 'o'}, {0x1EDB, 'o'}, {0x1EDF, 'o'}, {0x1EE1, 'o'}, {0x1EE3, 'o'}, // ơ
        {0x0075, 'u'}, {0x00F9, 'u'}, {0x00FA, 'u'}, {0x1EE7, 'u'}, {0x0169, 'u'}, {0x1EE5, 'u'}, // u
        {0x01B0, 'u'}, {0x1EEB, 'u'}, {0x1EE9, 'u'}, {0x1EED, 'u'}, {0x1EEF, 'u'}, {0x1EF1, 'u'}, // ư
        {0x0079, 'y'}, {0x1EF3, 'y'}, {0x00FD, 'y'}, {0x1EF7, 'y'}, {0x1EF9, 'y'}, {0x1EF5, 'y'}, // y
        {0x0111, 'd'} // đ
    };
} // namespace ẩn

// Thêm ký tự Unicode vào chuỗi
bool appendCodePoint(string& output, uint32_t codepoint) noexcept {
    if (codepoint > 0x10FFFF) return false;
    if (codepoint <= 0x7F) {
        output.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) {
        output.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        output.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else {
        output.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }
    return true;
}

// Đọc ký tự Unicode từ chuỗi
uint32_t readCodePoint(const string& input, size_t& index) noexcept {
    if (index >= input.size()) return REPLACEMENT_CHARACTER;
    unsigned char c = static_cast<unsigned char>(input[index]);
    if (c < 0x80) {
        ++index;
        return c;
    }
    if ((c & 0xE0) == 0xC0) {
        if (index + 1 >= input.size()) {
            ++index;
            return REPLACEMENT_CHARACTER;
        }
        unsigned char c1 = static_cast<unsigned char>(input[index + 1]);
        if ((c1 & 0xC0) != 0x80) {
            index += 2;
            return REPLACEMENT_CHARACTER;
        }
        uint32_t cp = ((c & 0x1F) << 6) | (c1 & 0x3F);
        index += 2;
        return cp;
    }
    if ((c & 0xF0) == 0xE0) {
        if (index + 2 >= input.size()) {
            ++index;
            return REPLACEMENT_CHARACTER;
        }
        unsigned char c1 = static_cast<unsigned char>(input[index + 1]);
        unsigned char c2 = static_cast<unsigned char>(input[index + 2]);
        if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80) {
            index += 3;
            return REPLACEMENT_CHARACTER;
        }
        uint32_t cp = ((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
        index += 3;
        return cp;
    }
    if ((c & 0xF8) == 0xF0) {
        if (index + 3 >= input.size()) {
            ++index;
            return REPLACEMENT_CHARACTER;
        }
        unsigned char c1 = static_cast<unsigned char>(input[index + 1]);
        unsigned char c2 = static_cast<unsigned char>(input[index + 2]);
        unsigned char c3 = static_cast<unsigned char>(input[index + 3]);
        if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) {
            index += 4;
            return REPLACEMENT_CHARACTER;
        }
        uint32_t cp = ((c & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
        index += 4;
        return cp;
    }
    ++index;
    return REPLACEMENT_CHARACTER;
}

// Kiểm tra ký tự có phải chữ cái tiếng Việt
bool isVietnameseLetter(uint32_t codepoint) noexcept {
    if ((codepoint >= 0x41 && codepoint <= 0x5A) || (codepoint >= 0x61 && codepoint <= 0x7A)) return true;
    if ((codepoint >= 0xC0 && codepoint <= 0xD6) || (codepoint >= 0xD8 && codepoint <= 0xFF)) return true;
    if (codepoint >= 0x0100 && codepoint <= 0x024F) return true;
    if (codepoint >= 0x1E00 && codepoint <= 0x1EFF) return true;
    return false;
}

// Phân loại ký tự
CharType getCharacterType(uint32_t codepoint) noexcept {
    if (codepoint <= 0x7F) {
        char c = static_cast<char>(codepoint);
        if (isAsciiLetter(c)) return CharType::ASCII_LETTER;
        if (isAsciiDigit(c)) return CharType::ASCII_DIGIT;
        if (isWhitespace(c)) return CharType::ASCII_SPACE;
        return CharType::INVALID;
    }
    if (isVietnameseLetter(codepoint)) return CharType::VIETNAMESE_LETTER;
    return CharType::INVALID;
}

// Chuyển ký tự thành HOA
uint32_t toUpper(uint32_t codepoint) noexcept {
    if (codepoint <= 0x7F && isAsciiLetter(static_cast<char>(codepoint))) {
        char c = static_cast<char>(codepoint);
        if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
        return codepoint;
    }
    auto it = LOWER_TO_UPPER.find(codepoint);
    return it != LOWER_TO_UPPER.end() ? it->second : codepoint;
}

// Chuyển ký tự thành thường
uint32_t toLower(uint32_t codepoint) noexcept {
    if (codepoint <= 0x7F && isAsciiLetter(static_cast<char>(codepoint))) {
        char c = static_cast<char>(codepoint);
        if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
        return codepoint;
    }
    auto it = UPPER_TO_LOWER.find(codepoint);
    return it != UPPER_TO_LOWER.end() ? it->second : codepoint;
}

// Chuẩn hóa chuỗi theo chế độ (hoa, thường, hoặc chữ đầu HOA)
string normalize(const string& input, NormalizationMode mode) {
    if (input.empty()) return "";
    string result;
    result.reserve(input.size());
    bool newWord = true;
    size_t index = 0;
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        CharType type = getCharacterType(cp);
        if (type == CharType::ASCII_SPACE) {
            if (!result.empty() && result.back() != ASCII_SPACE) {
                result.push_back(ASCII_SPACE);
            }
            newWord = true;
        } else if (type == CharType::ASCII_LETTER || type == CharType::VIETNAMESE_LETTER) {
            uint32_t converted = cp;
            if (mode == NormalizationMode::UPPER_CASE) converted = toUpper(cp);
            else if (mode == NormalizationMode::LOWER_CASE) converted = toLower(cp);
            else if (mode == NormalizationMode::TITLE_CASE) converted = newWord ? toUpper(cp) : toLower(cp);
            appendCodePoint(result, converted);
            newWord = false;
        } else {
            appendCodePoint(result, cp);
            newWord = false;
        }
    }
    while (!result.empty() && result.back() == ASCII_SPACE) result.pop_back();
    return result;
}

// Chuyển chuỗi thành dạng chữ đầu HOA
string toTitleCase(const string& input) {
    return normalize(input, NormalizationMode::TITLE_CASE);
}

// Chuẩn hóa và cắt khoảng trắng
string trimAndNormalize(const string& input) {
    return toTitleCase(input);
}

// Chia chuỗi thành danh sách từ
StringList* splitWords(const string& input) {
    StringList* list = new StringList();
    string currentWord;
    size_t index = 0;
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        CharType type = getCharacterType(cp);
        if (type == CharType::ASCII_SPACE) {
            if (!currentWord.empty()) {
                StringNode* newNode = new StringNode(currentWord);
                newNode->next = list->head;
                list->head = newNode;
                currentWord.clear();
            }
        } else if (type == CharType::ASCII_LETTER || type == CharType::VIETNAMESE_LETTER || type == CharType::ASCII_DIGIT) {
            size_t oldIndex = index - (index > 0 ? 1 : 0);
            size_t tempIndex = index;
            for (size_t i = 1; i <= 4 && tempIndex > 0; ++i) {
                --tempIndex;
                if ((static_cast<unsigned char>(input[tempIndex]) & 0xC0) != 0x80) {
                    oldIndex = tempIndex;
                    break;
                }
            }
            size_t charStart = oldIndex;
            readCodePoint(input, oldIndex);
            currentWord.append(input, charStart, oldIndex - charStart);
        }
    }
    if (!currentWord.empty()) {
        StringNode* newNode = new StringNode(currentWord);
        newNode->next = list->head;
        list->head = newNode;
    }
    return list;
}

// Nối danh sách từ thành chuỗi
string joinWords(const StringList* words, const string& separator) {
    if (!words || !words->head) return "";
    string result;
    StringNode* reversed = nullptr;
    StringNode* temp = words->head;
    while (temp) {
        StringNode* next = temp->next;
        temp->next = reversed;
        reversed = temp;
        temp = next;
    }
    result = reversed->word;
    StringNode* current = reversed->next;
    while (current) {
        result += separator + current->word;
        current = current->next;
    }
    temp = reversed;
    reversed = nullptr;
    while (temp) {
        StringNode* next = temp->next;
        temp->next = reversed;
        reversed = temp;
        temp = next;
    }
    return result;
}

// Giải phóng danh sách liên kết đơn
void FreeStringList(StringList* list) {
    if (!list) return;
    StringNode* current = list->head;
    while (current) {
        StringNode* temp = current;
        current = current->next;
        delete temp;
    }
    delete list;
}

// Bỏ dấu tiếng Việt
string removeAccents(const string& input) {
    string result;
    result.reserve(input.size());
    size_t index = 0;
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        if (cp <= 0x7F) {
            char c = static_cast<char>(cp);
            if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
            if (isAsciiLetter(c) || isAsciiDigit(c) || isWhitespace(c)) result.push_back(c);
        } else {
            auto it = ACCENT_MAP.find(toLower(cp));
            if (it != ACCENT_MAP.end()) result.push_back(it->second);
        }
    }
    return result;
}
// Thiết lập console để hỗ trợ UTF-8 trên Windows và locale chung
void initConsoleForUtf8() {
#if defined(_WIN32) || defined(_WIN64)
    // Enable UTF-8 code page for console I/O
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // Ensure stdout/stderr use UTF-8 on MSVCRT
    std::ios::sync_with_stdio(true);
    // Try to set wide locale
    std::setlocale(LC_ALL, "");
#else
    std::setlocale(LC_ALL, "");
#endif
}

// Chuyển toàn bộ chuỗi sang chữ thường (Unicode-aware)
std::string toLowerCase(const std::string& input) {
    return normalize(input, NormalizationMode::LOWER_CASE);
}

// Chuyển toàn bộ chuỗi sang chữ HOA (Unicode-aware)
std::string toUpperCase(const std::string& input) {
    return normalize(input, NormalizationMode::UPPER_CASE);
}
} // namespace VietnameseUtils