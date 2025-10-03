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

#include "../include/VietnameseUtils.h"
#include <cctype>
#include <algorithm>
#include <unordered_map>
#include <array>

using namespace std;

namespace VietnameseUtils {

// =============================================================================
// PRIVATE CONSTANTS VA LOOKUP TABLES
// =============================================================================

namespace {
    // Kiem tra la ky tu khong gian (space, tab, newline, etc.)
    constexpr bool isWhitespace(char c) noexcept { 
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; 
    }

    // Kiem tra la chu cai ASCII
    constexpr bool isAsciiLetter(char c) noexcept { 
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); 
    }

    constexpr bool isAsciiLower(char c) noexcept { return c >= 'a' && c <= 'z'; }
    constexpr bool isAsciiUpper(char c) noexcept { return c >= 'A' && c <= 'Z'; }
    constexpr bool isAsciiDigit(char c) noexcept { return c >= '0' && c <= '9'; }

    // Struct luu cap upper/lower case cho tieng Viet - optimized for lookup
    struct CasePair { 
        uint32_t lower; 
        uint32_t upper; 
        
        constexpr CasePair(uint32_t l, uint32_t u) noexcept : lower(l), upper(u) {}
    };

    //=========================================================================
    // UNICODE CASE CONVERSION TABLES
    // Sources: Unicode Consortium Official Data
    // - Case Folding: https://unicode.org/Public/UCD/latest/ucd/CaseFolding.txt
    // - Latin-1 Supplement: U+00C0-U+00FF 
    // - Latin Extended-A: U+0100-U+017F
    // - Latin Extended Additional: U+1E00-U+1EFF (Vietnamese specific)
    //=========================================================================
    
    // Simplified case conversion using unordered_map for easier maintenance
    // Mapping: lowercase → UPPERCASE (based on Unicode standard case folding)
    const unordered_map<uint32_t, uint32_t> LOWER_TO_UPPER = {
        // ASCII
        {'a', 'A'}, {'b', 'B'}, {'c', 'C'}, {'d', 'D'}, {'e', 'E'}, {'f', 'F'},
        {'g', 'G'}, {'h', 'H'}, {'i', 'I'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'},
        {'m', 'M'}, {'n', 'N'}, {'o', 'O'}, {'p', 'P'}, {'q', 'Q'}, {'r', 'R'},
        {'s', 'S'}, {'t', 'T'}, {'u', 'U'}, {'v', 'V'}, {'w', 'W'}, {'x', 'X'},
        {'y', 'Y'}, {'z', 'Z'},
        
        // Vietnamese accented characters
        {0x00E0, 0x00C0}, {0x00E1, 0x00C1}, {0x1EA3, 0x1EA2}, {0x00E3, 0x00C3}, {0x1EA1, 0x1EA0},
        {0x0103, 0x0102}, {0x1EB1, 0x1EB0}, {0x1EAF, 0x1EAE}, {0x1EB3, 0x1EB2}, {0x1EB5, 0x1EB4}, {0x1EB7, 0x1EB6},
        {0x00E2, 0x00C2}, {0x1EA7, 0x1EA6}, {0x1EA5, 0x1EA4}, {0x1EA9, 0x1EA8}, {0x1EAB, 0x1EAA}, {0x1EAD, 0x1EAC},
        {0x0111, 0x0110},
        {0x00E8, 0x00C8}, {0x00E9, 0x00C9}, {0x1EBB, 0x1EBA}, {0x1EBD, 0x1EBC}, {0x1EB9, 0x1EB8},
        {0x00EA, 0x00CA}, {0x1EC1, 0x1EC0}, {0x1EBF, 0x1EBE}, {0x1EC3, 0x1EC2}, {0x1EC5, 0x1EC4}, {0x1EC7, 0x1EC6},
        {0x00EC, 0x00CC}, {0x00ED, 0x00CD}, {0x1EC9, 0x1EC8}, {0x0129, 0x0128}, {0x1ECB, 0x1ECA},
        {0x00F2, 0x00D2}, {0x00F3, 0x00D3}, {0x1ECF, 0x1ECE}, {0x00F5, 0x00D5}, {0x1ECD, 0x1ECC},
        {0x00F4, 0x00D4}, {0x1ED3, 0x1ED2}, {0x1ED1, 0x1ED0}, {0x1ED5, 0x1ED4}, {0x1ED7, 0x1ED6}, {0x1ED9, 0x1ED8},
        {0x01A1, 0x01A0}, {0x1EDD, 0x1EDC}, {0x1EDB, 0x1EDA}, {0x1EDF, 0x1EDE}, {0x1EE1, 0x1EE0}, {0x1EE3, 0x1EE2},
        {0x00F9, 0x00D9}, {0x00FA, 0x00DA}, {0x1EE7, 0x1EE6}, {0x0169, 0x0168}, {0x1EE5, 0x1EE4},
        {0x01B0, 0x01AF}, {0x1EEB, 0x1EEA}, {0x1EE9, 0x1EE8}, {0x1EED, 0x1EEC}, {0x1EEF, 0x1EEE}, {0x1EF1, 0x1EF0},
        {0x1EF3, 0x1EF2}, {0x00FD, 0x00DD}, {0x1EF7, 0x1EF6}, {0x1EF9, 0x1EF8}, {0x1EF5, 0x1EF4}
    };

    const unordered_map<uint32_t, uint32_t> UPPER_TO_LOWER = {
        // ASCII
        {'A', 'a'}, {'B', 'b'}, {'C', 'c'}, {'D', 'd'}, {'E', 'e'}, {'F', 'f'},
        {'G', 'g'}, {'H', 'h'}, {'I', 'i'}, {'J', 'j'}, {'K', 'k'}, {'L', 'l'},
        {'M', 'm'}, {'N', 'n'}, {'O', 'o'}, {'P', 'p'}, {'Q', 'q'}, {'R', 'r'},
        {'S', 's'}, {'T', 't'}, {'U', 'u'}, {'V', 'v'}, {'W', 'w'}, {'X', 'x'},
        {'Y', 'y'}, {'Z', 'z'},
        
        // Vietnamese accented characters (reverse mapping)
        {0x00C0, 0x00E0}, {0x00C1, 0x00E1}, {0x1EA2, 0x1EA3}, {0x00C3, 0x00E3}, {0x1EA0, 0x1EA1},
        {0x0102, 0x0103}, {0x1EB0, 0x1EB1}, {0x1EAE, 0x1EAF}, {0x1EB2, 0x1EB3}, {0x1EB4, 0x1EB5}, {0x1EB6, 0x1EB7},
        {0x00C2, 0x00E2}, {0x1EA6, 0x1EA7}, {0x1EA4, 0x1EA5}, {0x1EA8, 0x1EA9}, {0x1EAA, 0x1EAB}, {0x1EAC, 0x1EAD},
        {0x0110, 0x0111},
        {0x00C8, 0x00E8}, {0x00C9, 0x00E9}, {0x1EBA, 0x1EBB}, {0x1EBC, 0x1EBD}, {0x1EB8, 0x1EB9},
        {0x00CA, 0x00EA}, {0x1EC0, 0x1EC1}, {0x1EBE, 0x1EBF}, {0x1EC2, 0x1EC3}, {0x1EC4, 0x1EC5}, {0x1EC6, 0x1EC7},
        {0x00CC, 0x00EC}, {0x00CD, 0x00ED}, {0x1EC8, 0x1EC9}, {0x0128, 0x0129}, {0x1ECA, 0x1ECB},
        {0x00D2, 0x00F2}, {0x00D3, 0x00F3}, {0x1ECE, 0x1ECF}, {0x00D5, 0x00F5}, {0x1ECC, 0x1ECD},
        {0x00D4, 0x00F4}, {0x1ED2, 0x1ED3}, {0x1ED0, 0x1ED1}, {0x1ED4, 0x1ED5}, {0x1ED6, 0x1ED7}, {0x1ED8, 0x1ED9},
        {0x01A0, 0x01A1}, {0x1EDC, 0x1EDD}, {0x1EDA, 0x1EDB}, {0x1EDE, 0x1EDF}, {0x1EE0, 0x1EE1}, {0x1EE2, 0x1EE3},
        {0x00D9, 0x00F9}, {0x00DA, 0x00FA}, {0x1EE6, 0x1EE7}, {0x0168, 0x0169}, {0x1EE4, 0x1EE5},
        {0x01AF, 0x01B0}, {0x1EEA, 0x1EEB}, {0x1EE8, 0x1EE9}, {0x1EEC, 0x1EED}, {0x1EEE, 0x1EEF}, {0x1EF0, 0x1EF1},
        {0x1EF2, 0x1EF3}, {0x00DD, 0x00FD}, {0x1EF6, 0x1EF7}, {0x1EF8, 0x1EF9}, {0x1EF4, 0x1EF5}
    };

    //=========================================================================
    // ACCENT REMOVAL MAPPING TABLE
    // Purpose: Convert Vietnamese accented characters to ASCII base characters
    // Sources: Vietnamese typography standards and Unicode normalization forms
    // Usage: For search functionality that ignores accents
    //=========================================================================
    
    // Accent removal mapping: tieng Viet → ASCII base characters
    // All Vietnamese diacritical marks are mapped to their base Latin letters
    const unordered_map<uint32_t, char> ACCENT_MAP = {
        // a, ă, â + dấu -> 'a'
        {0x0061, 'a'}, {0x00E0, 'a'}, {0x00E1, 'a'}, {0x1EA3, 'a'}, {0x00E3, 'a'}, {0x1EA1, 'a'},
        {0x0103, 'a'}, {0x1EB1, 'a'}, {0x1EAF, 'a'}, {0x1EB3, 'a'}, {0x1EB5, 'a'}, {0x1EB7, 'a'},
        {0x00E2, 'a'}, {0x1EA7, 'a'}, {0x1EA5, 'a'}, {0x1EA9, 'a'}, {0x1EAB, 'a'}, {0x1EAD, 'a'},
        
        // e, ê + dấu -> 'e'
        {0x0065, 'e'}, {0x00E8, 'e'}, {0x00E9, 'e'}, {0x1EBB, 'e'}, {0x1EBD, 'e'}, {0x1EB9, 'e'},
        {0x00EA, 'e'}, {0x1EC1, 'e'}, {0x1EBF, 'e'}, {0x1EC3, 'e'}, {0x1EC5, 'e'}, {0x1EC7, 'e'},
        
        // i + dấu -> 'i'
        {0x0069, 'i'}, {0x00EC, 'i'}, {0x00ED, 'i'}, {0x1EC9, 'i'}, {0x0129, 'i'}, {0x1ECB, 'i'},
        
        // o, ô, ơ + dấu -> 'o'
        {0x006F, 'o'}, {0x00F2, 'o'}, {0x00F3, 'o'}, {0x1ECF, 'o'}, {0x00F5, 'o'}, {0x1ECD, 'o'},
        {0x00F4, 'o'}, {0x1ED3, 'o'}, {0x1ED1, 'o'}, {0x1ED5, 'o'}, {0x1ED7, 'o'}, {0x1ED9, 'o'},
        {0x01A1, 'o'}, {0x1EDD, 'o'}, {0x1EDB, 'o'}, {0x1EDF, 'o'}, {0x1EE1, 'o'}, {0x1EE3, 'o'},
        
        // u, ư + dấu -> 'u'  
        {0x0075, 'u'}, {0x00F9, 'u'}, {0x00FA, 'u'}, {0x1EE7, 'u'}, {0x0169, 'u'}, {0x1EE5, 'u'},
        {0x01B0, 'u'}, {0x1EEB, 'u'}, {0x1EE9, 'u'}, {0x1EED, 'u'}, {0x1EEF, 'u'}, {0x1EF1, 'u'},
        
        // y + dấu -> 'y'
        {0x0079, 'y'}, {0x1EF3, 'y'}, {0x00FD, 'y'}, {0x1EF7, 'y'}, {0x1EF9, 'y'}, {0x1EF5, 'y'},
        
        // đ -> 'd'
        {0x0111, 'd'}
    };

} // anonymous namespace

// =============================================================================
// CORE UTF-8 VA UNICODE FUNCTIONS
// =============================================================================

bool appendCodePoint(string& output, uint32_t codepoint) noexcept {
    try {
        if (codepoint > 0x10FFFF) {
            return false; // Invalid Unicode codepoint
        }
        
        if (codepoint <= 0x7F) { 
            output.push_back(static_cast<char>(codepoint)); 
        }
        else if (codepoint <= 0x7FF) {
            output.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
            output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        else if (codepoint <= 0xFFFF) {
            output.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
            output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        else {
            // 4-byte UTF-8
            output.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
            output.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
            output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
            output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
        }
        return true;
    }
    catch (...) {
        return false;
    }
}

uint32_t readCodePoint(const string& input, size_t& index) noexcept {
    if (index >= input.size()) {
        return REPLACEMENT_CHARACTER;
    }
    
    unsigned char c = static_cast<unsigned char>(input[index]);
    
    if (c < 0x80) { // ASCII 1 byte
        ++index; 
        return c;
    }
    
    if ((c & 0xE0) == 0xC0) { // 2 bytes
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
    
    if ((c & 0xF0) == 0xE0) { // 3 bytes
        if (index + 2 >= input.size()) { 
            ++index; 
            return REPLACEMENT_CHARACTER; 
        }
        unsigned char c1 = static_cast<unsigned char>(input[index + 1]);
        unsigned char c2 = static_cast<unsigned char>(input[index + 2]);
        if (((c1 & 0xC0) != 0x80) || ((c2 & 0xC0) != 0x80)) { 
            index += 3; 
            return REPLACEMENT_CHARACTER; 
        }
        uint32_t cp = ((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
        index += 3; 
        return cp;
    }
    
    if ((c & 0xF8) == 0xF0) { // 4 bytes
        if (index + 3 >= input.size()) { 
            ++index; 
            return REPLACEMENT_CHARACTER; 
        }
        unsigned char c1 = static_cast<unsigned char>(input[index + 1]);
        unsigned char c2 = static_cast<unsigned char>(input[index + 2]);
        unsigned char c3 = static_cast<unsigned char>(input[index + 3]);
        if (((c1 & 0xC0) != 0x80) || ((c2 & 0xC0) != 0x80) || ((c3 & 0xC0) != 0x80)) { 
            index += 4; 
            return REPLACEMENT_CHARACTER; 
        }
        uint32_t cp = ((c & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
        index += 4; 
        return cp;
    }
    
    ++index; // Invalid byte
    return REPLACEMENT_CHARACTER;
}

bool isVietnameseLetter(uint32_t codepoint) noexcept {
    // ASCII letters
    if ((codepoint >= 0x41 && codepoint <= 0x5A) || (codepoint >= 0x61 && codepoint <= 0x7A)) {
        return true;
    }
    
    // Latin-1 Supplement: À..Ö, Ø..ö, ø..ÿ
    if ((codepoint >= 0xC0 && codepoint <= 0xD6) || 
        (codepoint >= 0xD8 && codepoint <= 0xF6) || 
        (codepoint >= 0xF8 && codepoint <= 0xFF)) {
        return true;
    }
    
    // Latin Extended-A/B: cho tiếng Việt và các ngôn ngữ Châu Âu
    if (codepoint >= 0x0100 && codepoint <= 0x024F) {
        return true;
    }
    
    // Latin Extended Additional: ặ, ẵ, ẳ, etc.
    if (codepoint >= 0x1E00 && codepoint <= 0x1EFF) {
        return true;
    }
    
    return false;
}

CharType getCharacterType(uint32_t codepoint) noexcept {
    if (codepoint <= 0x7F) {
        char c = static_cast<char>(codepoint);
        if (isAsciiLetter(c)) return CharType::ASCII_LETTER;
        if (isAsciiDigit(c)) return CharType::ASCII_DIGIT;
        if (isWhitespace(c)) return CharType::ASCII_SPACE;
        return CharType::ASCII_OTHER;
    }
    
    if (isVietnameseLetter(codepoint)) {
        return CharType::VIETNAMESE_LETTER;
    }
    
    if (codepoint == REPLACEMENT_CHARACTER) {
        return CharType::INVALID;
    }
    
    return CharType::UNICODE_OTHER;
}

size_t getCharacterCount(const string& input) noexcept {
    size_t count = 0;
    size_t index = 0;
    
    while (index < input.size()) {
        readCodePoint(input, index);
        ++count;
    }
    
    return count;
}

// =============================================================================
// CASE CONVERSION FUNCTIONS
// =============================================================================

uint32_t convertCase(uint32_t codepoint, bool toUpper) noexcept {
    // Handle ASCII first (optimization)
    if (codepoint <= 0x7F) {
        char c = static_cast<char>(codepoint);
        if (toUpper && isAsciiLower(c)) {
            return c - 'a' + 'A';
        }
        if (!toUpper && isAsciiUpper(c)) {
            return c - 'A' + 'a';
        }
        return codepoint;
    }
    
    // Use hash map lookup for Vietnamese characters
    if (toUpper) {
        auto it = LOWER_TO_UPPER.find(codepoint);
        return (it != LOWER_TO_UPPER.end()) ? it->second : codepoint;
    } else {
        auto it = UPPER_TO_LOWER.find(codepoint);
        return (it != UPPER_TO_LOWER.end()) ? it->second : codepoint;
    }
}

// =============================================================================
// STRING NORMALIZATION FUNCTIONS  
// =============================================================================

string normalize(const string& input, NormalizationMode mode) {
    if (input.empty()) return "";
    
    string result;
    result.reserve(input.size());
    
    size_t index = 0;
    bool newWord = true;
    
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        CharType type = getCharacterType(cp);
        
        switch (type) {
            case CharType::ASCII_SPACE:
            case CharType::ASCII_OTHER:
                if (cp == ASCII_SPACE) {
                    if (!result.empty() && result.back() != ASCII_SPACE) {
                        result.push_back(ASCII_SPACE);
                    }
                    newWord = true;
                } else {
                    appendCodePoint(result, cp);
                    newWord = (cp == '.' || cp == '!' || cp == '?');
                }
                break;
                
            case CharType::ASCII_LETTER:
            case CharType::VIETNAMESE_LETTER:
                {
                    uint32_t converted = cp;
                    switch (mode) {
                        case NormalizationMode::UPPER_CASE:
                            converted = toUpper(cp);
                            break;
                        case NormalizationMode::LOWER_CASE:
                            converted = toLower(cp);
                            break;
                        case NormalizationMode::TITLE_CASE:
                            converted = newWord ? toUpper(cp) : toLower(cp);
                            break;
                        case NormalizationMode::SENTENCE_CASE:
                            converted = newWord ? toUpper(cp) : toLower(cp);
                            break;
                    }
                    appendCodePoint(result, converted);
                    newWord = false;
                }
                break;
                
            case CharType::ASCII_DIGIT:
                appendCodePoint(result, cp);
                newWord = false;
                break;
                
            default:
                appendCodePoint(result, cp);
                break;
        }
    }
    
    // Remove trailing spaces
    while (!result.empty() && result.back() == ASCII_SPACE) {
        result.pop_back();
    }
    
    return result;
}

string toTitleCase(const string& input) {
    return normalize(input, NormalizationMode::TITLE_CASE);
}

string toUpperCase(const string& input) {
    return normalize(input, NormalizationMode::UPPER_CASE);
}

string toLowerCase(const string& input) {
    return normalize(input, NormalizationMode::LOWER_CASE);
}

string trimAndNormalize(const string& input) {
    return normalize(input, NormalizationMode::TITLE_CASE);
}

// =============================================================================
// SEARCH VA COMPARISON FUNCTIONS
// =============================================================================

string removeAccents(const string& input) {
    string result;
    result.reserve(input.size());
    
    size_t index = 0;
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        
        if (cp <= 0x7F) {
            char c = static_cast<char>(cp);
            if (isAsciiUpper(c)) {
                c = c - 'A' + 'a'; // Convert to lowercase
            }
            if (isAsciiLetter(c) || isAsciiDigit(c) || isWhitespace(c)) {
                result.push_back(c);
            }
        } else {
            auto it = ACCENT_MAP.find(toLower(cp));
            if (it != ACCENT_MAP.end()) {
                result.push_back(it->second);
            }
        }
    }
    
    return result;
}

bool compareIgnoreAccents(const string& str1, const string& str2) {
    return removeAccents(str1) == removeAccents(str2);
}

size_t findIgnoreAccents(const string& haystack, const string& needle) {
    string normalizedHaystack = removeAccents(haystack);
    string normalizedNeedle = removeAccents(needle);
    
    size_t pos = normalizedHaystack.find(normalizedNeedle);
    if (pos == string::npos) {
        return string::npos;
    }
    
    // Convert byte position back to character position in original string
    size_t charPos = 0;
    size_t bytePos = 0;
    size_t index = 0;
    
    while (index < haystack.size() && bytePos < pos) {
        size_t oldIndex = index;
        readCodePoint(haystack, index);
        size_t bytesRead = index - oldIndex;
        
        if (bytePos + bytesRead <= pos) {
            bytePos += bytesRead;
            ++charPos;
        } else {
            break;
        }
    }
    
    return charPos;
}

// =============================================================================
// VALIDATION FUNCTIONS
// =============================================================================

bool isValidVietnameseText(const string& input) noexcept {
    if (input.empty()) return true;
    
    size_t index = 0;
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        if (cp == REPLACEMENT_CHARACTER) {
            return false;
        }
        
        CharType type = getCharacterType(cp);
        if (type == CharType::INVALID) {
            return false;
        }
    }
    
    return true;
}

bool isValidUtf8(const string& input) noexcept {
    size_t index = 0;
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        if (cp == REPLACEMENT_CHARACTER) {
            return false;
        }
    }
    return true;
}

string fixUtf8Errors(const string& input) {
    string result;
    result.reserve(input.size());
    
    size_t index = 0;
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        appendCodePoint(result, cp); // Will use replacement char for invalid sequences
    }
    
    return result;
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

string truncateByCharacters(const string& input, size_t maxChars) {
    if (maxChars == 0) return "";
    
    string result;
    result.reserve(input.size());
    
    size_t charCount = 0;
    size_t index = 0;
    
    while (index < input.size() && charCount < maxChars) {
        size_t oldIndex = index;
        readCodePoint(input, index);
        
        if (charCount + 1 <= maxChars) {
            result.append(input, oldIndex, index - oldIndex);
            ++charCount;
        } else {
            break;
        }
    }
    
    return result;
}

vector<string> splitWords(const string& input) {
    vector<string> words;
    string currentWord;
    
    size_t index = 0;
    while (index < input.size()) {
        uint32_t cp = readCodePoint(input, index);
        CharType type = getCharacterType(cp);
        
        if (type == CharType::ASCII_SPACE || cp == ASCII_SPACE) {
            if (!currentWord.empty()) {
                words.push_back(move(currentWord));
                currentWord.clear();
            }
        } else if (type == CharType::ASCII_LETTER || 
                   type == CharType::VIETNAMESE_LETTER || 
                   type == CharType::ASCII_DIGIT) {
            size_t oldIndex = index - (index > 0 ? 1 : 0);
            // Go back to start of current character
            size_t tempIndex = index;
            for (size_t i = 1; i <= 4 && tempIndex > 0; ++i) {
                --tempIndex;
                if ((static_cast<unsigned char>(input[tempIndex]) & 0xC0) != 0x80) {
                    oldIndex = tempIndex;
                    break;
                }
            }
            
            // Recalculate current character size
            size_t charStart = oldIndex;
            readCodePoint(input, oldIndex);
            currentWord.append(input, charStart, oldIndex - charStart);
        }
    }
    
    if (!currentWord.empty()) {
        words.push_back(move(currentWord));
    }
    
    return words;
}

string joinWords(const vector<string>& words, const string& separator) {
    if (words.empty()) return "";
    
    string result = words[0];
    for (size_t i = 1; i < words.size(); ++i) {
        result += separator + words[i];
    }
    
    return result;
}

// =============================================================================
// BACKWARD COMPATIBILITY ALIASES  
// =============================================================================

// Provide old function names for backward compatibility
void appendCodePointUtf8(string& out, uint32_t cp) {
    appendCodePoint(out, cp);
}

uint32_t docCodePointUtf8(const string& s, size_t& i) {
    return readCodePoint(s, i);
}

bool laChuCaiLatinUnicode(uint32_t cp) {
    return isVietnameseLetter(cp);
}

uint32_t chuyenDoiCaseTiengViet(uint32_t cp, bool thanhHoa) {
    return convertCase(cp, thanhHoa);
}

uint32_t chuyenThanhHoaTiengViet(uint32_t cp) {
    return toUpper(cp);
}

uint32_t chuyenThanhThuongTiengViet(uint32_t cp) {
    return toLower(cp);
}

string tieuDeHoaTiengViet(const string& s) {
    return toTitleCase(s);
}

string chuanHoaTenTiengViet(const string& ten) {
    return toTitleCase(ten);
}

string chuanHoaChuoiTiengViet(const string& chuoi) {
    return trimAndNormalize(chuoi);
}

} // namespace VietnameseUtils