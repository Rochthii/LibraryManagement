#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <iostream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include <functional>
#include "../include/DauSach.h" // Để truy cập dsDauSach
#include "../include/DocGia.h"  // Để truy cập rootDocGia

// Global enum for language support
enum class Language { VI, EN };

namespace InputUtils {

const int MAX_ATTEMPTS = 5; // Số lần thử nhập lại tối đa


// Struct để lưu lỗi
struct Error {
    size_t position;
    char character;
};

// Hàm in danh sách lỗi (không dùng vector)
void InLoi(const Error* errors, int errorCount, const std::string& message_vi, const std::string& message_en, std::ostream& out, Language lang) {
    if (errorCount > 0) {
        out << "\a\n";
        if (lang == Language::VI) out << "Lỗi: ";
        else out << "Error: ";
        for (int i = 0; i < errorCount; ++i) {
            out << "Ký tự '" << errors[i].character << "' tại vị trí " << errors[i].position << " ";
            if (lang == Language::VI) out << message_vi;
            else out << message_en;
            if (i < errorCount - 1) out << ", ";
            else out << "!";
        }
        if (lang == Language::VI) out << " Vui lòng nhập lại.\n";
        else out << " Please re-enter.\n";
    }
}

// Hàm 1: Nhập chuỗi số (ISBN, MaThe)
std::string NhapChuoiSo(size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI) {
    std::string number;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        number.clear();
        if (lang == Language::VI)
            out << "Nhập số (" << minLength << "-" << maxLength << " ký tự): ";
        else
            out << "Enter number (" << minLength << "-" << maxLength << " chars): ";
        std::getline(in, number);
        // Kiểm tra chuỗi rỗng hoặc toàn khoảng trắng
        if (number.empty()) {
            if (lang == Language::VI)
                out << "Lỗi: Không được để trống!\n";
            else
                out << "Error: Input cannot be empty!\n";
            attempts++;
            continue;
        }
        if (number.find_first_not_of(' ') == std::string::npos) {
            if (lang == Language::VI)
                out << "Lỗi: Chỉ chứa khoảng trắng, không hợp lệ!\n";
            else
                out << "Error: Input contains only spaces, invalid!\n";
            attempts++;
            continue;
        }
        if (number.length() > 100) {
            if (lang == Language::VI)
                out << "Lỗi: Chuỗi nhập quá dài (tối đa 100 ký tự)!\n";
            else
                out << "Error: Input string too long (max 100 chars)!\n";
            attempts++;
            continue;
        }
        Error* errors = new Error[number.length()];
        int errorCount = 0;
        for (size_t i = 0; i < number.length(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(number[i]))) {
                errors[errorCount++] = {i + 1, number[i]};
            }
        }
        InLoi(errors, errorCount, "không phải số", "not a digit", out, lang);
        bool hasError = errorCount > 0;
        delete[] errors;
        if (hasError || number.length() < minLength || number.length() > maxLength) {
            if (number.length() < minLength) {
                if (lang == Language::VI)
                    out << "Lỗi: Số quá ngắn (tối thiểu " << minLength << " ký tự)!\n";
                else
                    out << "Error: Number too short (min " << minLength << " chars)!\n";
            } else if (number.length() > maxLength) {
                if (lang == Language::VI)
                    out << "Lỗi: Số quá dài (tối đa " << maxLength << " ký tự)!\n";
                else
                    out << "Error: Number too long (max " << maxLength << " chars)!\n";
            }
            attempts++;
            if (attempts < MAX_ATTEMPTS) {
                if (lang == Language::VI)
                    out << "Còn " << (MAX_ATTEMPTS - attempts) << " lần thử.\n";
                else
                    out << (MAX_ATTEMPTS - attempts) << " attempts left.\n";
            }
            continue;
        }
        break;
    }
    if (attempts >= MAX_ATTEMPTS) {
        if (lang == Language::VI)
            throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
        else
            throw std::invalid_argument("Error: Exceeded maximum allowed attempts!");
    }
    return number;
}

// Hàm 2: Nhập tên (TenSach, TacGia, TheLoai, Ho, Ten) với viết hoa đầu và loại bỏ non-ASCII
std::string NhapTen(size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI) {
    std::string str;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        str.clear();
        if (lang == Language::VI)
            out << "Nhập tên (" << minLength << "-" << maxLength << " ký tự): ";
        else
            out << "Enter name (" << minLength << "-" << maxLength << " chars): ";
        std::getline(in, str);
        
        Error* errors = new Error[str.length()];
        int errorCount = 0;
        for (size_t i = 0; i < str.length(); ++i) {
            unsigned char uc = static_cast<unsigned char>(str[i]);
            if ((!std::isalpha(uc) && str[i] != ' ') || uc > 127) {
                errors[errorCount++] = {i + 1, str[i]};
            }
        }
        InLoi(errors, errorCount, "không phải chữ cái ASCII hoặc khoảng trắng", "not an ASCII letter or space", out, lang);
        bool hasError = errorCount > 0;
        delete[] errors;
        // Loại bỏ khoảng trắng thừa và ký tự non-ASCII
        std::string cleaned;
        bool lastWasSpace = true;
        for (char c : str) {
            unsigned char uc = static_cast<unsigned char>(c);
            if ((std::isalpha(uc) || (c == ' ' && !lastWasSpace)) && uc <= 127) {
                cleaned += c;
                lastWasSpace = (c == ' ');
            }
        }
        // Xóa khoảng trắng đầu/cuối và nhiều khoảng trắng liên tiếp
        cleaned.erase(cleaned.begin(), std::find_if(cleaned.begin(), cleaned.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        cleaned.erase(std::find_if(cleaned.rbegin(), cleaned.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), cleaned.end());
        cleaned.erase(std::unique(cleaned.begin(), cleaned.end(), [](char a, char b) { return a == ' ' && b == ' '; }), cleaned.end());
        // Viết hoa chữ cái đầu mỗi từ
        std::string result;
        bool capitalize = true;
        for (char c : cleaned) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (capitalize && std::isalpha(uc)) {
                result += std::toupper(uc);
                capitalize = false;
            } else {
                result += std::tolower(uc);
            }
            if (c == ' ') capitalize = true;
        }
        if (errorCount > 0 || result.length() < minLength || result.length() > maxLength) {
            if (result.length() < minLength) {
                if (lang == Language::VI)
                    out << "Lỗi: Tên quá ngắn (tối thiểu " << minLength << " ký tự)!\n";
                else
                    out << "Error: Name too short (min " << minLength << " chars)!\n";
            } else if (result.length() > maxLength) {
                if (lang == Language::VI)
                    out << "Lỗi: Tên quá dài (tối đa " << maxLength << " ký tự)!\n";
                else
                    out << "Error: Name too long (max " << maxLength << " chars)!\n";
            }
            attempts++;
            if (attempts < MAX_ATTEMPTS) {
                if (lang == Language::VI)
                    out << "Còn " << (MAX_ATTEMPTS - attempts) << " lần thử.\n";
                else
                    out << (MAX_ATTEMPTS - attempts) << " attempts left.\n";
            }
            continue;
        }
        if (result.empty()) {
            if (lang == Language::VI)
                out << "Lỗi: Tên không được rỗng sau khi xử lý!\n";
            else
                out << "Error: Name cannot be empty after processing!\n";
            attempts++;
            if (attempts < MAX_ATTEMPTS) {
                if (lang == Language::VI)
                    out << "Còn " << (MAX_ATTEMPTS - attempts) << " lần thử.\n";
                else
                    out << (MAX_ATTEMPTS - attempts) << " attempts left.\n";
            }
            continue;
        }
        return result;
    }
    if (lang == Language::VI)
        throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
    else
        throw std::invalid_argument("Error: Exceeded maximum allowed attempts!");
}

// Hàm 3: Nhập mã (maSach)
std::string NhapMa(size_t minLength, size_t maxLength, std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI) {
    std::string code;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        code.clear();
        if (lang == Language::VI)
            out << "Nhập mã (" << minLength << "-" << maxLength << " ký tự): ";
        else
            out << "Enter code (" << minLength << "-" << maxLength << " chars): ";
        std::getline(in, code);
        
        Error* errors = new Error[code.length()];
        int errorCount = 0;
        for (size_t i = 0; i < code.length(); ++i) {
            unsigned char uc = static_cast<unsigned char>(code[i]);
            if ((!std::isalnum(uc) && code[i] != '-' && code[i] != '_') || uc > 127) {
                errors[errorCount++] = {i + 1, code[i]};
            }
        }
        InLoi(errors, errorCount, "không hợp lệ trong mã (chỉ chữ, số, -, _)", "invalid in code (only letters, digits, -, _)", out, lang);
        bool hasError = errorCount > 0;
        delete[] errors;
        // Chuẩn hóa mã: chữ cái in hoa
        for (char& c : code) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (std::isalpha(uc)) {
                c = std::toupper(uc);
            }
        }
        if (hasError || code.length() < minLength || code.length() > maxLength) {
            if (code.length() < minLength) {
                if (lang == Language::VI)
                    out << "Lỗi: Mã quá ngắn (tối thiểu " << minLength << " ký tự)!\n";
                else
                    out << "Error: Code too short (min " << minLength << " chars)!\n";
            } else if (code.length() > maxLength) {
                if (lang == Language::VI)
                    out << "Lỗi: Mã quá dài (tối đa " << maxLength << " ký tự)!\n";
                else
                    out << "Error: Code too long (max " << maxLength << " chars)!\n";
            }
            attempts++;
            if (attempts < MAX_ATTEMPTS) {
                if (lang == Language::VI)
                    out << "Còn " << (MAX_ATTEMPTS - attempts) << " lần thử.\n";
                else
                    out << (MAX_ATTEMPTS - attempts) << " attempts left.\n";
            }
            continue;
        }
        break;
    }
    if (attempts >= MAX_ATTEMPTS) {
        if (lang == Language::VI)
            throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
        else
            throw std::invalid_argument("Error: Exceeded maximum allowed attempts!");
    }
    return code;
}

// Hàm 4: Nhập số nguyên (SoTrang, NamXuatBan, soLuongSach)
int NhapSoNguyen(int minVal, int maxVal, std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI) {
    std::string input;
    int number;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        input.clear();
        if (lang == Language::VI)
            out << "Nhập số nguyên (" << minVal << "-" << maxVal << "): ";
        else
            out << "Enter integer (" << minVal << "-" << maxVal << "): ";
        std::getline(in, input);
        // Cho phép số âm, kiểm tra hợp lệ
        std::string trimmed = input;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
        // Kiểm tra chuỗi rỗng hoặc toàn khoảng trắng
        if (trimmed.empty()) {
            if (lang == Language::VI)
                out << "Lỗi: Không được để trống!\n";
            else
                out << "Error: Input cannot be empty!\n";
            attempts++;
            continue;
        }
        if (trimmed.find_first_not_of(' ') == std::string::npos) {
            if (lang == Language::VI)
                out << "Lỗi: Chỉ chứa khoảng trắng, không hợp lệ!\n";
            else
                out << "Error: Input contains only spaces, invalid!\n";
            attempts++;
            continue;
        }
        if (trimmed.length() > 20) {
            if (lang == Language::VI)
                out << "Lỗi: Chuỗi nhập quá dài (tối đa 20 ký tự)!\n";
            else
                out << "Error: Input string too long (max 20 chars)!\n";
            attempts++;
            continue;
        }
        int minusCount = 0;
        Error* errors = new Error[trimmed.length()];
        int errorCount = 0;
        for (size_t i = 0; i < trimmed.length(); ++i) {
            unsigned char uc = static_cast<unsigned char>(trimmed[i]);
            if (trimmed[i] == '-') {
                minusCount++;
                if (i != 0) {
                    errors[errorCount++] = {i + 1, trimmed[i]};
                }
                continue;
            }
            if (!std::isdigit(uc)) {
                errors[errorCount++] = {i + 1, trimmed[i]};
            }
        }
        if (minusCount > 1) {
            if (lang == Language::VI)
                out << "Lỗi: Chỉ được có một dấu '-' ở đầu chuỗi số!\n";
            else
                out << "Error: Only one '-' allowed at the start of the number!\n";
            attempts++;
            delete[] errors;
            continue;
        }
        if (errorCount > 0) {
            if (lang == Language::VI)
                out << "Lỗi: Có ký tự không phải số trong chuỗi!\n";
            else
                out << "Error: There are non-digit characters in the input!\n";
        }
        InLoi(errors, errorCount, "không phải số", "not a digit", out, lang);
        bool hasError = errorCount > 0;
        delete[] errors;
        if (hasError) {
            attempts++;
            if (attempts < MAX_ATTEMPTS) {
                if (lang == Language::VI)
                    out << "Còn " << (MAX_ATTEMPTS - attempts) << " lần thử.\n";
                else
                    out << (MAX_ATTEMPTS - attempts) << " attempts left.\n";
            }
            continue;
        }
        try {
            size_t idx = 0;
            long long val = std::stoll(trimmed, &idx);
            if (idx != trimmed.length() || val < INT_MIN || val > INT_MAX) throw std::out_of_range("");
            number = static_cast<int>(val);
            if (number < minVal || number > maxVal) {
                if (lang == Language::VI)
                    out << "Lỗi: Giá trị ngoài khoảng cho phép (" << minVal << " đến " << maxVal << ")!\n";
                else
                    out << "Error: Value out of allowed range (" << minVal << " to " << maxVal << ")!\n";
                attempts++;
                if (attempts < MAX_ATTEMPTS) {
                    if (lang == Language::VI)
                        out << "Còn " << (MAX_ATTEMPTS - attempts) << " lần thử.\n";
                    else
                        out << (MAX_ATTEMPTS - attempts) << " attempts left.\n";
                }
                continue;
            }
        } catch (...) {
            if (lang == Language::VI)
                out << "Lỗi: Không phải số nguyên hợp lệ hoặc bị tràn (quá lớn/nhỏ)!\n";
            else
                out << "Error: Not a valid integer or overflow (too large/small)!\n";
            attempts++;
            if (attempts < MAX_ATTEMPTS) {
                if (lang == Language::VI)
                    out << "Còn " << (MAX_ATTEMPTS - attempts) << " lần thử.\n";
                else
                    out << (MAX_ATTEMPTS - attempts) << " attempts left.\n";
            }
            continue;
        }
        break;
    }
    if (attempts >= MAX_ATTEMPTS) {
        if (lang == Language::VI)
            throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
        else
            throw std::invalid_argument("Error: Exceeded maximum allowed attempts!");
    }
    return number;
}

// Hàm 5: Kiểm tra trùng ISBN
bool KiemTraTrungISBN(const std::string& ISBN, std::ostream& out = std::cout, Language lang = Language::VI) {
    for (int i = 0; i < soLuongDauSach; i++) {
        if (dsDauSach[i] && dsDauSach[i]->ISBN == ISBN) {
            if (lang == Language::VI)
                out << "Lỗi: ISBN " << ISBN << " đã tồn tại!\n";
            else
                out << "Error: ISBN " << ISBN << " already exists!\n";
            return true;
        }
    }
    return false;
}

// Hàm 6: Kiểm tra maSach hợp lệ và trạng thái
// mode: 0 (tồn tại), 1 (mượn: TrangThai=0), 2 (trả: TrangThai=1)
bool KiemTramaSach(const std::string& maSach, int mode, std::ostream& out = std::cout, Language lang = Language::VI) {
    for (int i = 0; i < soLuongDauSach; i++) {
        if (dsDauSach[i]) {
            for (DanhMucSach* sach = dsDauSach[i]->dms; sach; sach = sach->next) {
                if (sach->maSach == maSach) {
                    if (mode == 0) return true;
                    if (mode == 1 && sach->trangThai == 0) return true;
                    if (mode == 2 && sach->trangThai == 1) return true;
                    if (lang == Language::VI)
                        out << "Lỗi: maSach " << maSach << " không hợp lệ (trạng thái: " << sach->trangThai << ")!\n";
                    else
                        out << "Error: maSach " << maSach << " invalid (status: " << sach->trangThai << ")!\n";
                    return false;
                }
            }
        }
    }
    if (lang == Language::VI)
        out << "Lỗi: maSach " << maSach << " không tồn tại!\n";
    else
        out << "Error: maSach " << maSach << " does not exist!\n";
    return false;
}

// Hàm 7: Kiểm tra chuỗi không rỗng
bool KiemTraChuoiRong(const std::string& str, const std::string& fieldName, std::ostream& out = std::cout, Language lang = Language::VI) {
    if (str.empty()) {
        if (lang == Language::VI)
            out << "Lỗi: " << fieldName << " không được rỗng!\n";
        else
            out << "Error: " << fieldName << " cannot be empty!\n";
        return false;
    }
    return true;
}

// Hàm 8: Chuyển chuỗi về in thường
std::string ChuyenInThuong(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

// Hàm 9: Nhập Phái (Nam/Nữ) với chuẩn hóa
std::string NhapPhai(std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI) {
    std::string phai;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        phai.clear();
        if (lang == Language::VI)
            out << "Nhập phái (Nam/Nữ): ";
        else
            out << "Enter gender (Nam/Nữ): ";
        std::getline(in, phai);
        Error* errors = new Error[phai.length()];
        int errorCount = 0;
        for (size_t i = 0; i < phai.length(); ++i) {
            unsigned char uc = static_cast<unsigned char>(phai[i]);
            if (!std::isalpha(uc) && !std::isspace(uc) || uc > 127) {
                errors[errorCount++] = {i + 1, phai[i]};
            }
        }
        InLoi(errors, errorCount, "không phải chữ cái ASCII", "not an ASCII letter", out, lang);
        bool hasError = errorCount > 0;
        delete[] errors;
        // Chuẩn hóa: loại bỏ khoảng trắng, viết hoa đầu
        std::string cleaned;
        for (char c : phai) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (std::isalpha(uc) && uc <= 127) {
                cleaned += c;
            }
        }
        if (cleaned.empty()) {
            if (lang == Language::VI)
                out << "Lỗi: Phái không được rỗng!\n";
            else
                out << "Error: Gender cannot be empty!\n";
            attempts++;
            continue;
        }
        std::string standardized = ChuyenInThuong(cleaned);
        if (standardized == "nam" || standardized == "nữ" || standardized == "nu") {
            if (standardized == "nam") return "Nam";
            return "Nữ";
        }
        if (lang == Language::VI)
            out << "Lỗi: Phái phải là 'Nam' hoặc 'Nữ'!\n";
        else
            out << "Error: Gender must be 'Nam' or 'Nữ'!\n";
        attempts++;
        if (attempts < MAX_ATTEMPTS) {
            if (lang == Language::VI)
                out << "Còn " << (MAX_ATTEMPTS - attempts) << " lần thử.\n";
            else
                out << (MAX_ATTEMPTS - attempts) << " attempts left.\n";
        }
    }
    if (lang == Language::VI)
        throw std::invalid_argument("Lỗi: Quá số lần nhập sai cho phép!");
    else
        throw std::invalid_argument("Error: Exceeded maximum allowed attempts!");
}

// Hàm 10: Kiểm tra trạng thái thẻ (0/1)
bool KiemTraTrangThaiThe(int trangThai, std::ostream& out = std::cout, Language lang = Language::VI) {
    if (trangThai == 0 || trangThai == 1) {
        return true;
    }
    if (lang == Language::VI)
        out << "Lỗi: Trạng thái thẻ phải là 0 (khóa) hoặc 1 (hoạt động)!\n";
    else
        out << "Error: Card status must be 0 (locked) or 1 (active)!\n";
    return false;
}

// Hàm 11: Kiểm tra mã thẻ độc giả tồn tại
bool KiemTraMaThe(const std::string& maThe, std::ostream& out = std::cout, Language lang = Language::VI) {
    int maTheInt = 0;
    try {
        maTheInt = std::stoi(maThe);
    } catch (...) {
        if (lang == Language::VI)
            out << "Lỗi: Mã thẻ không hợp lệ (không phải số)!\n";
        else
            out << "Error: Card ID is invalid (not a number)!\n";
        return false;
    }
    DocGia* docGia = timDocGia(maTheInt);
    if (docGia) return true;
    if (lang == Language::VI)
        out << "Lỗi: Mã thẻ " << maThe << " không tồn tại!\n";
    else
        out << "Error: Card ID " << maThe << " does not exist!\n";
    return false;
}

// Hàm 12: Sinh mã thẻ ngẫu nhiên không trùng
std::string SinhMaTheNgauNhien(size_t length, std::ostream& out = std::cout, Language lang = Language::VI) {
    static bool initialized = false;
    if (!initialized) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        initialized = true;
    }
    
    const char digits[] = "0123456789";
    std::string maThe;
    int attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        maThe.clear();
        for (size_t i = 0; i < length; ++i) {
            maThe += digits[std::rand() % 10];
        }
        int maTheInt = 0;
        try {
            maTheInt = std::stoi(maThe);
        } catch (...) {
            // Nếu không chuyển được sang số, sinh lại mã mới
            attempts++;
            continue;
        }
        if (!timDocGia(maTheInt)) return maThe;
        attempts++;
        if (lang == Language::VI)
            out << "Cảnh báo: Mã thẻ " << maThe << " đã tồn tại, thử lại lần " << attempts << ".\n";
        else
            out << "Warning: Card ID " << maThe << " already exists, retry attempt " << attempts << ".\n";
    }
    if (lang == Language::VI)
        throw std::runtime_error("Lỗi: Không thể sinh mã thẻ không trùng sau 5 lần thử!");
    else
        throw std::runtime_error("Error: Cannot generate unique card ID after 5 attempts!");
}

// Hàm 13: Kiểm tra định dạng ISBN (10 hoặc 13 chữ số)
bool KiemTraISBN(const std::string& ISBN, std::ostream& out = std::cout, Language lang = Language::VI) {
    if (ISBN.length() != 10 && ISBN.length() != 13) {
        if (lang == Language::VI)
            out << "Lỗi: ISBN phải có 10 hoặc 13 chữ số!\n";
        else
            out << "Error: ISBN must have 10 or 13 digits!\n";
        return false;
    }
    for (char c : ISBN) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            if (lang == Language::VI)
                out << "Lỗi: ISBN chỉ được chứa chữ số!\n";
            else
                out << "Error: ISBN must contain only digits!\n";
            return false;
        }
    }
    return true;
}

// Hàm 14: Kiểm tra tổng số bản sao
bool KiemTraTongSoBanSao(const std::string& ISBN, int soLuongSach, std::ostream& out = std::cout, Language lang = Language::VI) {
    int currentCount = 0;
    for (int i = 0; i < soLuongDauSach; i++) {
        if (dsDauSach[i] && dsDauSach[i]->ISBN == ISBN) {
            for (DanhMucSach* sach = dsDauSach[i]->dms; sach; sach = sach->next) {
                currentCount++;
            }
            break;
        }
    }
    if (currentCount + soLuongSach > 1000) {
        if (lang == Language::VI)
            out << "Lỗi: Tổng số bản sao vượt quá 1000!\n";
        else
            out << "Error: Total number of copies exceeds 1000!\n";
        return false;
    }
    return true;
}

// Hàm 15: Kiểm tra trùng maSach toàn cục
bool KiemTraTrungmaSach(const std::string& maSach, std::ostream& out = std::cout, Language lang = Language::VI) {
    for (int i = 0; i < soLuongDauSach; i++) {
        if (dsDauSach[i]) {
            for (DanhMucSach* sach = dsDauSach[i]->dms; sach; sach = sach->next) {
                if (sach->maSach == maSach) {
                    if (lang == Language::VI)
                        out << "Lỗi: maSach " << maSach << " đã tồn tại!\n";
                    else
                        out << "Error: maSach " << maSach << " already exists!\n";
                    return true;
                }
            }
        }
    }
    return false;
}

// Hàm 16: Nhập lựa chọn sắp xếp (1: tên+họ, 2: mã thẻ)
int NhapLuaChonSapXep(std::istream& in = std::cin, std::ostream& out = std::cout, Language lang = Language::VI) {
    return NhapSoNguyen(1, 2, in, out, lang);
}

// Hàm 17: Kiểm tra số sách đang mượn
bool KiemTraSoSachDangMuon(const std::string& maThe, std::ostream& out = std::cout, Language lang = Language::VI) {
    int maTheInt = 0;
    try {
        maTheInt = std::stoi(maThe);
    } catch (...) {
        if (lang == Language::VI)
            out << "Lỗi: Mã thẻ không hợp lệ (không phải số)!\n";
        else
            out << "Error: Card ID is invalid (not a number)!\n";
        return false;
    }
    DocGia* docGia = timDocGia(maTheInt);
    if (!docGia) {
        if (lang == Language::VI)
            out << "Lỗi: Mã thẻ " << maThe << " không tồn tại!\n";
        else
            out << "Error: Card ID " << maThe << " does not exist!\n";
        return false;
    }
    int count = 0;
    for (MuonTra* mt = docGia->dsMuonTra; mt; mt = mt->next) {
        if (mt->trangThai == 0) count++; // Đang mượn
    }
    if (count >= 3) {
        if (lang == Language::VI)
            out << "Lỗi: Độc giả đã mượn tối đa 3 sách!\n";
        else
            out << "Error: Reader has borrowed maximum 3 books!\n";
        return false;
    }
    return true;
}

// Hàm 18: Kiểm tra sách quá hạn
bool KiemTraQuaHan(const std::string& maThe, std::ostream& out = std::cout, Language lang = Language::VI) {
    int maTheInt = 0;
    try {
        maTheInt = std::stoi(maThe);
    } catch (...) {
        if (lang == Language::VI)
            out << "Lỗi: Mã thẻ không hợp lệ (không phải số)!\n";
        else
            out << "Error: Card ID is invalid (not a number)!\n";
        return false;
    }
    DocGia* docGia = timDocGia(maTheInt);
    if (!docGia) {
        if (lang == Language::VI)
            out << "Lỗi: Mã thẻ " << maThe << " không tồn tại!\n";
        else
            out << "Error: Card ID " << maThe << " does not exist!\n";
        return false;
    }
    time_t now = time(nullptr);
    
    // Helper function to convert date string (DD/MM/YYYY) to time_t
    auto dateStringToTimeT = [](const std::string& dateStr) -> time_t {
        struct tm tm = {};
        int day, month, year;
        if (sscanf(dateStr.c_str(), "%d/%d/%d", &day, &month, &year) == 3) {
            tm.tm_mday = day;
            tm.tm_mon = month - 1;  // tm_mon is 0-11
            tm.tm_year = year - 1900;  // tm_year is years since 1900
            return mktime(&tm);
        }
        return 0;  // Invalid date
    };
    
    for (MuonTra* mt = docGia->dsMuonTra; mt; mt = mt->next) {
        time_t ngayMuonTime = dateStringToTimeT(mt->ngayMuon);
        if (mt->trangThai == 0 && ngayMuonTime > 0 && difftime(now, ngayMuonTime) > 7 * 86400) {
            if (lang == Language::VI)
                out << "Lỗi: Độc giả có sách quá hạn!\n";
            else
                out << "Error: Reader has overdue books!\n";
            return false;
        }
    }
    return true;
}

// Hàm 19: Kiểm tra ngày hiện tại
bool KiemTraNgayHienTai(time_t ngay, std::ostream& out = std::cout, Language lang = Language::VI) {
    time_t now = time(nullptr);
    if (ngay > now || ngay < 0) {
        if (lang == Language::VI)
            out << "Lỗi: Ngày không hợp lệ!\n";
        else
            out << "Error: Invalid date!\n";
        return false;
    }
    return true;
}

// Hàm 20: Kiểm tra lượt mượn
bool KiemTraLuotMuon(int luotMuon, std::ostream& out = std::cout, Language lang = Language::VI) {
    if (luotMuon >= 0) return true;
    if (lang == Language::VI)
        out << "Lỗi: Lượt mượn phải lớn hơn hoặc bằng 0!\n";
    else
        out << "Error: Borrow count must be >= 0!\n";
    return false;
}

// Hàm 21: Sinh mã sách từ ISBN và số thứ tự với kiểm tra lỗi và tự động sửa
std::string sinhMaSach(std::string ISBN, int soThuTu, std::ostream& out = std::cout, Language lang = Language::VI) {
    // Tự động xóa khoảng trắng đầu cuối
    ISBN.erase(ISBN.begin(), std::find_if(ISBN.begin(), ISBN.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    ISBN.erase(std::find_if(ISBN.rbegin(), ISBN.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), ISBN.end());
    
    // Kiểm tra chuỗi rỗng sau khi trim
    if (ISBN.empty()) {
        if (lang == Language::VI)
            out << "Lỗi: ISBN không được rỗng!\n";
        else
            out << "Error: ISBN cannot be empty!\n";
        throw std::invalid_argument("ISBN cannot be empty");
    }
    
    // Tự động xóa các ký tự không phải số và dấu gạch ngang
    std::string cleanISBN;
    bool hasNonDigit = false;
    for (char c : ISBN) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            cleanISBN += c;
        } else if (c != '-' && c != ' ') {
            hasNonDigit = true;
        }
    }
    
    if (hasNonDigit) {
        if (lang == Language::VI)
            out << "Tự động sửa: Đã loại bỏ các ký tự không hợp lệ khỏi ISBN.\n";
        else
            out << "Auto-fix: Removed invalid characters from ISBN.\n";
    }
    
    if (cleanISBN.empty()) {
        if (lang == Language::VI)
            out << "Lỗi: ISBN không chứa chữ số nào!\n";
        else
            out << "Error: ISBN contains no digits!\n";
        throw std::invalid_argument("ISBN contains no digits");
    }
    
    // Kiểm tra số thứ tự và tự động sửa nếu <= 0
    if (soThuTu <= 0) {
        soThuTu = 1;
        if (lang == Language::VI)
            out << "Tự động sửa: Số thứ tự được đặt thành 1.\n";
        else
            out << "Auto-fix: Serial number set to 1.\n";
    }
    
    std::string maSach = cleanISBN + "-" + std::to_string(soThuTu);
    
    // Kiểm tra trùng mã sách và tự động tăng số thứ tự
    int attempts = 0;
    while (KiemTraTrungmaSach(maSach, out, lang) && attempts < 100) {
        attempts++;
        soThuTu++;
        maSach = cleanISBN + "-" + std::to_string(soThuTu);
    }
    
    if (attempts > 0) {
        if (lang == Language::VI)
            out << "Tự động sửa: Mã sách được điều chỉnh thành " << maSach << " để tránh trùng lặp.\n";
        else
            out << "Auto-fix: Book code adjusted to " << maSach << " to avoid duplication.\n";
    }
    
    return maSach;
}

// Hàm 22: Sinh mã thẻ từ root với kiểm tra lỗi
int sinhMaThe(DocGia* root, std::ostream& out = std::cout, Language lang = Language::VI) {
    if (!root) {
        if (lang == Language::VI)
            out << "Cảnh báo: Danh sách độc giả rỗng, bắt đầu từ mã thẻ 1!\n";
        else
            out << "Warning: Reader list empty, starting from card ID 1!\n";
        return 1;
    }
    
    int maxMaThe = 0;
    int count = 0;
    // Helper function to traverse BST and find max maThe
    std::function<void(DocGia*)> traverseBST = [&](DocGia* node) {
        if (!node) return;
        count++;
        if (node->maThe > maxMaThe) {
            maxMaThe = node->maThe;
        }
        // Kiểm tra giới hạn số lượng độc giả
        if (count >= 10000) {
            if (lang == Language::VI)
                out << "Cảnh báo: Đã có quá nhiều độc giả (>= 10000)!\n";
            else
                out << "Warning: Too many readers (>= 10000)!\n";
            return; // Exit traversal early
        }
        traverseBST(node->left);
        traverseBST(node->right);
    };
    
    traverseBST(root);
    
    int newMaThe = maxMaThe + 1;
    
    // Kiểm tra overflow
    if (newMaThe <= maxMaThe) {
        if (lang == Language::VI)
            throw std::overflow_error("Lỗi: Mã thẻ bị tràn số!");
        else
            throw std::overflow_error("Error: Card ID overflow!");
    }
    
    if (lang == Language::VI)
        out << "Sinh mã thẻ mới: " << newMaThe << "\n";
    else
        out << "Generated new card ID: " << newMaThe << "\n";
    
    return newMaThe;
}
}

// Hàm 23: Kiểm tra quá hạn từ ngày mượn với validation và tự động sửa
bool kiemTraQuaHan(std::string ngayMuon, std::ostream& out = std::cout, Language lang = Language::VI) {
    // Tự động xóa khoảng trắng đầu cuối
    ngayMuon.erase(ngayMuon.begin(), std::find_if(ngayMuon.begin(), ngayMuon.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    ngayMuon.erase(std::find_if(ngayMuon.rbegin(), ngayMuon.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), ngayMuon.end());
    
    // Kiểm tra chuỗi rỗng
    if (ngayMuon.empty()) {
        if (lang == Language::VI)
            out << "Lỗi: Ngày mượn không được rỗng!\n";
        else
            out << "Error: Borrow date cannot be empty!\n";
        return false;
    }
    
    // Tự động sửa định dạng ngày từ DD-MM-YYYY, DD.MM.YYYY thành DD/MM/YYYY
    bool autoFixed = false;
    if (ngayMuon.find('-') != std::string::npos) {
        std::replace(ngayMuon.begin(), ngayMuon.end(), '-', '/');
        autoFixed = true;
    }
    if (ngayMuon.find('.') != std::string::npos) {
        std::replace(ngayMuon.begin(), ngayMuon.end(), '.', '/');
        autoFixed = true;
    }
    
    if (autoFixed) {
        if (lang == Language::VI)
            out << "Tự động sửa: Định dạng ngày được chuyển thành " << ngayMuon << "\n";
        else
            out << "Auto-fix: Date format converted to " << ngayMuon << "\n";
    }
    
    // Tự động thêm số 0 đầu cho ngày/tháng nếu cần (d/m/yyyy -> dd/mm/yyyy)
    size_t firstSlash = ngayMuon.find('/');
    size_t secondSlash = ngayMuon.find('/', firstSlash + 1);
    
    if (firstSlash != std::string::npos && secondSlash != std::string::npos) {
        std::string day = ngayMuon.substr(0, firstSlash);
        std::string month = ngayMuon.substr(firstSlash + 1, secondSlash - firstSlash - 1);
        std::string year = ngayMuon.substr(secondSlash + 1);
        
        bool needPadding = false;
        if (day.length() == 1) {
            day = "0" + day;
            needPadding = true;
        }
        if (month.length() == 1) {
            month = "0" + month;
            needPadding = true;
        }
        
        if (needPadding) {
            ngayMuon = day + "/" + month + "/" + year;
            if (lang == Language::VI)
                out << "Tự động sửa: Thêm số 0 đầu cho ngày/tháng: " << ngayMuon << "\n";
            else
                out << "Auto-fix: Added leading zeros to day/month: " << ngayMuon << "\n";
        }
    }
    
    // Kiểm tra độ dài chuỗi ngày sau khi sửa
    if (ngayMuon.length() != 10) {
        if (lang == Language::VI)
            out << "Lỗi: Ngày mượn phải có định dạng DD/MM/YYYY!\n";
        else
            out << "Error: Borrow date must be in DD/MM/YYYY format!\n";
        return false;
    }
    
    // Parse ngày mượn (format DD/MM/YYYY)
    struct tm tm_muon = {};
    int day, month, year;
    
    if (sscanf(ngayMuon.c_str(), "%d/%d/%d", &day, &month, &year) != 3) {
        if (lang == Language::VI)
            out << "Lỗi: Định dạng ngày không hợp lệ! Sử dụng DD/MM/YYYY\n";
        else
            out << "Error: Invalid date format! Use DD/MM/YYYY\n";
        return false;
    }
    
    // Tự động sửa năm 2 chữ số thành 4 chữ số (YY -> 20YY hoặc 19YY)
    if (year < 100) {
        if (year <= 30) {
            year += 2000; // 00-30 -> 2000-2030
        } else {
            year += 1900; // 31-99 -> 1931-1999
        }
        if (lang == Language::VI)
            out << "Tự động sửa: Năm được chuyển thành " << year << "\n";
        else
            out << "Auto-fix: Year converted to " << year << "\n";
    }
    
    // Kiểm tra tính hợp lệ của ngày tháng năm
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2100) {
        if (lang == Language::VI)
            out << "Lỗi: Ngày tháng năm không hợp lệ!\n";
        else
            out << "Error: Invalid day/month/year!\n";
        return false;
    }
    
    tm_muon.tm_mday = day;
    tm_muon.tm_mon = month - 1; // tháng tính từ 0
    tm_muon.tm_year = year - 1900; // năm tính từ 1900
    
    time_t time_muon = mktime(&tm_muon);
    if (time_muon == -1) {
        if (lang == Language::VI)
            out << "Lỗi: Không thể chuyển đổi ngày mượn!\n";
        else
            out << "Error: Cannot convert borrow date!\n";
        return false;
    }
    
    time_t now = time(nullptr);
    double diff = difftime(now, time_muon);
    bool quaHan = diff > (7 * 24 * 60 * 60); // > 7 ngày
    
    if (quaHan) {
        int soNgayQuaHan = static_cast<int>(diff / (24 * 60 * 60)) - 7;
        if (lang == Language::VI)
            out << "Cảnh báo: Sách đã quá hạn " << soNgayQuaHan << " ngày!\n";
        else
            out << "Warning: Book is overdue by " << soNgayQuaHan << " days!\n";
    }
    
    return quaHan;
}

// Hàm 24: Lấy ngày hiện tại với kiểm tra lỗi
std::string layNgayHienTai(std::ostream& out = std::cout, Language lang = Language::VI) {
    time_t now = time(nullptr);
    if (now == -1) {
        if (lang == Language::VI)
            throw std::runtime_error("Lỗi: Không thể lấy thời gian hiện tại!");
        else
            throw std::runtime_error("Error: Cannot get current time!");
    }
    
    tm* ltm = localtime(&now);
    if (!ltm) {
        if (lang == Language::VI)
            throw std::runtime_error("Lỗi: Không thể chuyển đổi thời gian!");
        else
            throw std::runtime_error("Error: Cannot convert time!");
    }
    
    char buffer[11];
    size_t result = strftime(buffer, sizeof(buffer), "%d/%m/%Y", ltm);
    if (result == 0) {
        if (lang == Language::VI)
            throw std::runtime_error("Lỗi: Không thể định dạng ngày!");
        else
            throw std::runtime_error("Error: Cannot format date!");
    }
    
    std::string ngayHienTai(buffer);
    
    if (lang == Language::VI)
        out << "Ngày hiện tại: " << ngayHienTai << "\n";
    else
        out << "Current date: " << ngayHienTai << "\n";
    
    return ngayHienTai;
}

// Hàm 25: Tính số ngày quá hạn với validation đầy đủ và tự động sửa
int tinhSoNgayQuaHan(std::string ngayMuon, std::ostream& out = std::cout, Language lang = Language::VI) {
    // Tự động xóa khoảng trắng đầu cuối
    ngayMuon.erase(ngayMuon.begin(), std::find_if(ngayMuon.begin(), ngayMuon.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    ngayMuon.erase(std::find_if(ngayMuon.rbegin(), ngayMuon.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), ngayMuon.end());
    
    // Kiểm tra chuỗi rỗng
    if (ngayMuon.empty()) {
        if (lang == Language::VI)
            out << "Lỗi: Ngày mượn không được rỗng!\n";
        else
            out << "Error: Borrow date cannot be empty!\n";
        return -1;
    }
    
    // Tự động sửa định dạng ngày từ DD-MM-YYYY, DD.MM.YYYY thành DD/MM/YYYY
    bool autoFixed = false;
    if (ngayMuon.find('-') != std::string::npos) {
        std::replace(ngayMuon.begin(), ngayMuon.end(), '-', '/');
        autoFixed = true;
    }
    if (ngayMuon.find('.') != std::string::npos) {
        std::replace(ngayMuon.begin(), ngayMuon.end(), '.', '/');
        autoFixed = true;
    }
    
    if (autoFixed) {
        if (lang == Language::VI)
            out << "Tự động sửa: Định dạng ngày được chuyển thành " << ngayMuon << "\n";
        else
            out << "Auto-fix: Date format converted to " << ngayMuon << "\n";
    }
    
    // Tự động thêm số 0 đầu cho ngày/tháng nếu cần (d/m/yyyy -> dd/mm/yyyy)
    size_t firstSlash = ngayMuon.find('/');
    size_t secondSlash = ngayMuon.find('/', firstSlash + 1);
    
    if (firstSlash != std::string::npos && secondSlash != std::string::npos) {
        std::string day = ngayMuon.substr(0, firstSlash);
        std::string month = ngayMuon.substr(firstSlash + 1, secondSlash - firstSlash - 1);
        std::string year = ngayMuon.substr(secondSlash + 1);
        
        bool needPadding = false;
        if (day.length() == 1) {
            day = "0" + day;
            needPadding = true;
        }
        if (month.length() == 1) {
            month = "0" + month;
            needPadding = true;
        }
        
        if (needPadding) {
            ngayMuon = day + "/" + month + "/" + year;
            if (lang == Language::VI)
                out << "Tự động sửa: Thêm số 0 đầu cho ngày/tháng: " << ngayMuon << "\n";
            else
                out << "Auto-fix: Added leading zeros to day/month: " << ngayMuon << "\n";
        }
    }
    
    // Kiểm tra độ dài chuỗi ngày sau khi sửa
    if (ngayMuon.length() != 10) {
        if (lang == Language::VI)
            out << "Lỗi: Ngày mượn phải có định dạng DD/MM/YYYY!\n";
        else
            out << "Error: Borrow date must be in DD/MM/YYYY format!\n";
        return -1;
    }
    
    // Parse ngày mượn (format DD/MM/YYYY)
    struct tm tm_muon = {};
    int day, month, year;
    
    if (sscanf(ngayMuon.c_str(), "%d/%d/%d", &day, &month, &year) != 3) {
        if (lang == Language::VI)
            out << "Lỗi: Định dạng ngày không hợp lệ! Sử dụng DD/MM/YYYY\n";
        else
            out << "Error: Invalid date format! Use DD/MM/YYYY\n";
        return -1;
    }
    
    // Tự động sửa năm 2 chữ số thành 4 chữ số
    if (year < 100) {
        if (year <= 30) {
            year += 2000; // 00-30 -> 2000-2030
        } else {
            year += 1900; // 31-99 -> 1931-1999
        }
        if (lang == Language::VI)
            out << "Tự động sửa: Năm được chuyển thành " << year << "\n";
        else
            out << "Auto-fix: Year converted to " << year << "\n";
    }
    
    // Kiểm tra tính hợp lệ của ngày tháng năm
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900 || year > 2100) {
        if (lang == Language::VI)
            out << "Lỗi: Ngày tháng năm không hợp lệ!\n";
        else
            out << "Error: Invalid day/month/year!\n";
        return -1;
    }
    
    tm_muon.tm_mday = day;
    tm_muon.tm_mon = month - 1; // tháng tính từ 0
    tm_muon.tm_year = year - 1900; // năm tính từ 1900
    
    time_t time_muon = mktime(&tm_muon);
    if (time_muon == -1) {
        if (lang == Language::VI)
            out << "Lỗi: Không thể chuyển đổi ngày mượn!\n";
        else
            out << "Error: Cannot convert borrow date!\n";
        return -1;
    }
    
    time_t now = time(nullptr);
    if (now == -1) {
        if (lang == Language::VI)
            out << "Lỗi: Không thể lấy thời gian hiện tại!\n";
        else
            out << "Error: Cannot get current time!\n";
        return -1;
    }
    
    double diff = difftime(now, time_muon);
    int soNgayDaMuon = static_cast<int>(diff / (24 * 60 * 60));
    int soNgayQuaHan = (soNgayDaMuon > 7) ? (soNgayDaMuon - 7) : 0;
    
    if (soNgayQuaHan > 0) {
        if (lang == Language::VI)
            out << "Thông tin: Sách đã quá hạn " << soNgayQuaHan << " ngày (đã mượn " << soNgayDaMuon << " ngày).\n";
        else
            out << "Info: Book is overdue by " << soNgayQuaHan << " days (borrowed for " << soNgayDaMuon << " days).\n";
    } else {
        if (lang == Language::VI)
            out << "Thông tin: Sách chưa quá hạn (đã mượn " << soNgayDaMuon << " ngày).\n";
        else
            out << "Info: Book is not overdue (borrowed for " << soNgayDaMuon << " days).\n";
    }
    
    return soNgayQuaHan;
}

#endif // INPUT_UTILS_H