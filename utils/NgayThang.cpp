#include "include/NgayThang.h"
#include "include/ThongBao.h"
#include "include/Constants.h"
#include <ctime>
#include <iomanip>
#include <sstream>

int LayNamHienTai() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    return t->tm_year + 1900;
}

// Lay ngay hien tai dang chuoi
std::string layNgayHienTai(std::ostream& out) {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[11];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    return std::string(buf);
}

// Chuyển chuỗi ngày sang time_t
time_t ChuyenNgaySangTimeT(const std::string& ngay, std::ostream& out) {
    int day, month, year;
    if (!phanTichNgay(ngay, day, month, year)) {
        thongBao(out, "Ngày không đúng định dạng: " + ngay, LOI);
        return -1;
    }
    // Khởi tạo cấu trúc tm một cách an toàn và rõ ràng
    std::tm t{}; 
    t.tm_mday = day;
    t.tm_mon  = month - 1;     // tm_mon chạy từ 0..11
    t.tm_year = year - 1900;   // tm_year là năm kể từ 1900
    t.tm_hour = 0;
    t.tm_min  = 0;
    t.tm_sec  = 0;
    t.tm_isdst = -1;

    time_t result = mktime(&t);
    if (result == -1) {
        thongBao(out, "Không thể chuyển ngày sang time_t: " + ngay, LOI);
    }
    return result;
}

int tinhSoNgayQuaHan(const std::string& ngayMuon, std::ostream& out) {
    time_t now = time(nullptr);
    time_t tMuon = ChuyenNgaySangTimeT(ngayMuon, out);
    if (tMuon == -1) return -1;
    double diff = difftime(now, tMuon) / (60 * 60 * 24);
    int soNgayQuaHan = static_cast<int>(diff) - SO_NGAY_MUON_TOI_DA;
    return soNgayQuaHan > 0 ? soNgayQuaHan : 0;
}

// Phân tích chuỗi ngày dd/mm/yyyy
bool phanTichNgay(const std::string& ngay, int& d, int& m, int& y) {
    if (ngay.length() != 10 || ngay[2] != '/' || ngay[5] != '/') return false;
    try {
        d = std::stoi(ngay.substr(0, 2));
        m = std::stoi(ngay.substr(3, 2));
        y = std::stoi(ngay.substr(6, 4));
        if (d < 1 || d > 31 || m < 1 || m > 12 || y < NAM_XUAT_BAN_MIN) return false;
        return true;
    } catch (...) {
        return false;
    }
}