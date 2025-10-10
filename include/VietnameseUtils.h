#ifndef VIETNAMESE_UTILS_H
#define VIETNAMESE_UTILS_H

#include <string>

namespace VietnameseUtils {

// Loại ký tự (phân biệt chữ, số, khoảng trắng)
enum class CharType {
    ASCII_LETTER,      // Chữ cái ASCII (a-z, A-Z)
    ASCII_DIGIT,       // Số ASCII (0-9)
    ASCII_SPACE,       // Khoảng trắng
    VIETNAMESE_LETTER, // Chữ tiếng Việt (à, á, ạ,...)
    INVALID            // Ký tự không hợp lệ
};

// Chế độ chuẩn hóa chuỗi
enum class NormalizationMode {
    UPPER_CASE,   // Chuyển thành HOA
    LOWER_CASE,   // Chuyển thành thường
    TITLE_CASE    // Chữ cái đầu mỗi từ HOA
};

constexpr char ASCII_SPACE = ' ';           // Ký tự khoảng trắng
constexpr uint32_t REPLACEMENT_CHARACTER = 0xFFFD; // Ký tự thay thế khi lỗi UTF-8

// Cấu trúc node cho danh sách liên kết đơn lưu chuỗi
struct StringNode {
    std::string word; // Từ
    StringNode* next; // Con trỏ tới node tiếp theo
    StringNode(const std::string& w) : word(w), next(nullptr) {}
};

// Cấu trúc danh sách liên kết đơn
struct StringList {
    StringNode* head; // Con trỏ đầu danh sách
    StringList() : head(nullptr) {}
};

// Hàm xử lý Unicode
bool appendCodePoint(std::string& output, uint32_t codepoint) noexcept;
uint32_t readCodePoint(const std::string& input, size_t& index) noexcept;
bool isVietnameseLetter(uint32_t codepoint) noexcept;
CharType getCharacterType(uint32_t codepoint) noexcept;
uint32_t toUpper(uint32_t codepoint) noexcept;
uint32_t toLower(uint32_t codepoint) noexcept;

// Hàm chuẩn hóa chuỗi
std::string normalize(const std::string& input, NormalizationMode mode);
std::string toTitleCase(const std::string& input);
std::string trimAndNormalize(const std::string& input);

// Hàm xử lý danh sách từ
StringList* splitWords(const std::string& input);
std::string joinWords(const StringList* words, const std::string& separator);
void FreeStringList(StringList* list);

// Hàm bỏ dấu tiếng Việt (cho tìm kiếm)
std::string removeAccents(const std::string& input);

// Khởi tạo console để hiển thị UTF-8 (Windows) và thiết lập locale chung
void initConsoleForUtf8();

// Helper chuyển toàn bộ chuỗi sang chữ thường/hoa (Unicode-aware)
std::string toLowerCase(const std::string& input);
std::string toUpperCase(const std::string& input);

} // namespace VietnameseUtils

#endif