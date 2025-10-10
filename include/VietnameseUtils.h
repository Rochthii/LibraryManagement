#ifndef VIETNAMESE_UTILS_H
#define VIETNAMESE_UTILS_H

#include <string>
#include <cstdint>
#include <vector>

/**
 * @brief Namespace cho cac tien ich xu ly tieng Viet va Unicode
 * 
 * Module nay cung cap cac ham xu ly UTF-8, Unicode va cac thao tac
 * chuyen doi case, chuan hoa cho tieng Viet.
 * 
 * @note Unicode Data Sources:
 * - Unicode Standard: https://unicode.org/charts/PDF/U0000.pdf
 * - Vietnamese ranges: U+0080-U+00FF, U+0100-U+017F, U+1E00-U+1EFF  
 * - Case mapping: https://unicode.org/Public/UCD/latest/ucd/CaseFolding.txt
 * - UTF-8 spec: RFC 3629 (https://tools.ietf.org/html/rfc3629)
 * 
 * @author LibraryManagement Team
 * @version 2.1.0 (Professional Grade)
 * @date 2025-10-01
 * @copyright Unicode data © Unicode, Inc. Used under Unicode License Agreement
 */
namespace VietnameseUtils {

// =============================================================================
// CONSTANTS VA TYPES
// =============================================================================

/** @brief Ma Unicode replacement character cho loi UTF-8 */
constexpr uint32_t REPLACEMENT_CHARACTER = 0xFFFD;

/** @brief Ky tu space ASCII */
constexpr char ASCII_SPACE = ' ';

/** @brief Cac loai ky tu Unicode */
enum class CharType {
    ASCII_LETTER,       ///< Chu cai ASCII (a-z, A-Z)
    ASCII_DIGIT,        ///< Chu so ASCII (0-9)  
    ASCII_SPACE,        ///< Khoang trang ASCII
    ASCII_OTHER,        ///< Ky tu ASCII khac
    VIETNAMESE_LETTER,  ///< Chu cai tieng Viet co dau
    UNICODE_OTHER,      ///< Ky tu Unicode khac
    INVALID             ///< Ky tu khong hop le
};

/** @brief Cac che do chuan hoa */
enum class NormalizationMode {
    TITLE_CASE,         ///< Viet Hoa Chu Dau Moi Tu
    UPPER_CASE,         ///< VIET HOA TAT CA
    LOWER_CASE,         ///< viet thuong tat ca
    SENTENCE_CASE       ///< Viet hoa chu dau cau
};

// =============================================================================
// CORE UTF-8 VA UNICODE FUNCTIONS
// =============================================================================

/**
 * @brief Them mot Unicode codepoint vao chuoi UTF-8
 * @param[out] output Chuoi dich de them vao
 * @param[in] codepoint Ma Unicode can them (0x0 - 0x10FFFF)
 * @return true neu thanh cong, false neu codepoint khong hop le
 */
bool appendCodePoint(std::string& output, uint32_t codepoint) noexcept;

/**
 * @brief Doc mot Unicode codepoint tu chuoi UTF-8
 * @param[in] input Chuoi UTF-8 nguon
 * @param[in,out] index Vi tri hien tai, se duoc cap nhat sau khi doc
 * @return Ma Unicode da doc, hoac REPLACEMENT_CHARACTER neu loi
 */
uint32_t readCodePoint(const std::string& input, size_t& index) noexcept;

/**
 * @brief Kiem tra xem codepoint co phai chu cai Latin/Vietnamese khong
 * @param[in] codepoint Ma Unicode can kiem tra
 * @return true neu la chu cai Latin ho tro tieng Viet
 */
bool isVietnameseLetter(uint32_t codepoint) noexcept;

/**
 * @brief Xac dinh loai cua mot ky tu Unicode
 * @param[in] codepoint Ma Unicode can phan tich  
 * @return Loai ky tu tuong ung
 */
CharType getCharacterType(uint32_t codepoint) noexcept;

/**
 * @brief Dem so luong ky tu Unicode trong chuoi UTF-8
 * @param[in] input Chuoi UTF-8 can dem
 * @return So luong ky tu Unicode (khong phai byte)
 */
size_t getCharacterCount(const std::string& input) noexcept;

// =============================================================================
// CASE CONVERSION FUNCTIONS
// =============================================================================

/**
 * @brief Chuyen doi case cho ky tu tieng Viet 
 * @param[in] codepoint Ma Unicode nguon
 * @param[in] toUpper true = chuyen thanh hoa, false = chuyen thanh thuong
 * @return Ma Unicode sau khi chuyen doi
 */
uint32_t convertCase(uint32_t codepoint, bool toUpper) noexcept;

/**
 * @brief Chuyen ky tu thanh chu hoa tieng Viet
 * @param[in] codepoint Ma Unicode can chuyen doi
 * @return Ma Unicode chu hoa tuong ung
 */
inline uint32_t toUpper(uint32_t codepoint) noexcept {
    return convertCase(codepoint, true);
}

/**
 * @brief Chuyen ky tu thanh chu thuong tieng Viet  
 * @param[in] codepoint Ma Unicode can chuyen doi
 * @return Ma Unicode chu thuong tuong ung
 */
inline uint32_t toLower(uint32_t codepoint) noexcept {
    return convertCase(codepoint, false);
}

// =============================================================================
// STRING NORMALIZATION FUNCTIONS  
// =============================================================================

/**
 * @brief Chuan hoa chuoi theo che do chi dinh
 * @param[in] input Chuoi nguon
 * @param[in] mode Che do chuan hoa
 * @return Chuoi da chuan hoa
 */
std::string normalize(const std::string& input, NormalizationMode mode);

/**
 * @brief Viet hoa tieu de tieng Viet (Chu Dau Moi Tu Viet Hoa)
 * @param[in] input Chuoi nguon
 * @return Chuoi da format title case
 */
std::string toTitleCase(const std::string& input);

/**
 * @brief Chuyen toan bo chuoi thanh chu hoa
 * @param[in] input Chuoi nguon  
 * @return Chuoi chu hoa
 */
std::string toUpperCase(const std::string& input);

/**
 * @brief Chuyen toan bo chuoi thanh chu thuong
 * @param[in] input Chuoi nguon
 * @return Chuoi chu thuong  
 */
std::string toLowerCase(const std::string& input);

/**
 * @brief Loai bo khoang trang thua va chuan hoa
 * @param[in] input Chuoi nguon
 * @return Chuoi da loai bo khoang trang thua
 */
std::string trimAndNormalize(const std::string& input);

// =============================================================================
// SEARCH VA COMPARISON FUNCTIONS
// =============================================================================

/**
 * @brief Loai bo dau tieng Viet va chuyen ve ASCII de tim kiem
 * @param[in] input Chuoi tieng Viet co dau
 * @return Chuoi ASCII khong dau (a,e,i,o,u,y,d)
 */
std::string removeAccents(const std::string& input);

/**
 * @brief So sanh hai chuoi khong phan biet dau va case
 * @param[in] str1 Chuoi thu nhat
 * @param[in] str2 Chuoi thu hai  
 * @return true neu hai chuoi tuong duong
 */
bool compareIgnoreAccents(const std::string& str1, const std::string& str2);

/**
 * @brief Tim kiem substring khong phan biet dau va case
 * @param[in] haystack Chuoi cha
 * @param[in] needle Chuoi con can tim
 * @return Vi tri tim thay (string::npos neu khong tim thay)
 */
size_t findIgnoreAccents(const std::string& haystack, const std::string& needle);

// =============================================================================
// VALIDATION FUNCTIONS
// =============================================================================

/**
 * @brief Kiem tra chuoi co chua ky tu tieng Viet hop le khong
 * @param[in] input Chuoi can kiem tra
 * @return true neu chuoi hop le
 */
bool isValidVietnameseText(const std::string& input) noexcept;

/**
 * @brief Kiem tra chuoi UTF-8 co hop le khong
 * @param[in] input Chuoi can kiem tra
 * @return true neu UTF-8 hop le
 */
bool isValidUtf8(const std::string& input) noexcept;

/**
 * @brief Sua loi UTF-8 trong chuoi
 * @param[in] input Chuoi co the chua loi UTF-8
 * @return Chuoi da sua loi (thay the bang replacement character)
 */
std::string fixUtf8Errors(const std::string& input);

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Cat chuoi Unicode theo so luong ky tu (khong phai byte)
 * @param[in] input Chuoi nguon
 * @param[in] maxChars So ky tu toi da
 * @return Chuoi da cat
 */
std::string truncateByCharacters(const std::string& input, size_t maxChars);

/**
 * @brief Tach chuoi thanh cac tu rieng biet
 * @param[in] input Chuoi nguon
 * @return Vector cac tu da tach
 */
std::vector<std::string> splitWords(const std::string& input);

/**
 * @brief Noi cac tu thanh chuoi voi phan cach
 * @param[in] words Vector cac tu
 * @param[in] separator Ky tu phan cach (mac dinh la space)
 * @return Chuoi da noi
 */
std::string joinWords(const std::vector<std::string>& words, const std::string& separator = " ");

/**
 * @brief Khởi tạo console / locale để hỗ trợ UTF-8 trên cả Windows và POSIX.
 *
 * - Trên Windows: cố gắng chuyển code page sang UTF-8 (CP_UTF8) và bật
 *   ANSI VT processing khi có thể.
 * - Trên macOS/Linux: gọi setlocale(LC_ALL, "") và thiết lập locale cho iostream.
 *
 * Hàm này được đặt trong VietnameseUtils để tách phần platform-specific
 * khỏi `main.cpp` và các file khác.
 */
void initConsoleForUtf8() noexcept;

// -----------------------------------------------------------------------------
// Backward-compatibility wrappers (inline) - expose legacy names used elsewhere
// -----------------------------------------------------------------------------
// These are thin forwarding functions to the canonical API above. They are
// defined inline to avoid duplicate-symbol/linker issues and to allow gradual
// migration from old names to the new API.

inline void appendCodePointUtf8(std::string& out, uint32_t cp) noexcept {
    appendCodePoint(out, cp);
}

inline uint32_t docCodePointUtf8(const std::string& s, size_t& i) noexcept {
    return readCodePoint(s, i);
}

inline bool laChuCaiLatinUnicode(uint32_t cp) noexcept {
    return isVietnameseLetter(cp);
}

inline uint32_t chuyenDoiCaseTiengViet(uint32_t cp, bool thanhHoa) noexcept {
    return convertCase(cp, thanhHoa);
}

inline uint32_t chuyenThanhHoaTiengViet(uint32_t cp) noexcept {
    return toUpper(cp);
}

inline uint32_t chuyenThanhThuongTiengViet(uint32_t cp) noexcept {
    return toLower(cp);
}

inline std::string tieuDeHoaTiengViet(const std::string& s) {
    return toTitleCase(s);
}

inline std::string chuanHoaTenTiengViet(const std::string& ten) {
    return toTitleCase(ten);
}

inline std::string chuanHoaChuoiTiengViet(const std::string& chuoi) {
    return trimAndNormalize(chuoi);
}

} // namespace VietnameseUtils

#endif // VIETNAMESE_UTILS_H