#include "NgayThang.h"
#include "ThongBao.h"
#include "Constants.h"
#include <ctime>
#include <iomanip>
#include <sstream>

// Lay nam hien tai tu he thong
int LayNamHienTai() {
    time_t now = time(nullptr); 
    struct tm* t = localtime(&now);
    return t->tm_year + 1900;
}

// Lay ngay hien tai theo dang chuoi "dd/mm/yyyy"
std::string layNgayHienTai(std::ostream& out) {
    (void)out; 
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char buf[11];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
    return std::string(buf);
}

time_t ChuyenNgaySangTimeT(const std::string& ngay, std::ostream& out) {
    int day, month, year;
    // Buoc 1: Phan tich chuoi ngay thang nam
    if (!phanTichNgay(ngay, day, month, year)) {
        // Cap nhat thong bao khong dau
        thongBao(out, "Ngay khong dung dinh dang: " + ngay, LOI);
        return -1; // Tra ve -1 neu loi
    }
    // Buoc 2: Dien thong tin vao cau truc tm
    std::tm t{}; // Khoi tao cau truc tm bang 0
    t.tm_mday = day;
    t.tm_mon  = month - 1;     // tm_mon bat dau tu 0 (thang 1 la 0)
    t.tm_year = year - 1900;   // tm_year bat dau tu 1900
    t.tm_hour = 0;             // Dat gio, phut, giay = 0 de tinh tu dau ngay
    t.tm_min  = 0;
    t.tm_sec  = 0;
    t.tm_isdst = -1;           // De he thong tu dong xac dinh DST

    // Buoc 3: Chuyen doi cau truc tm sang time_t
    time_t result = mktime(&t);
    if (result == -1) {
        // Cap nhat thong bao khong dau
        thongBao(out, "Khong the chuyen ngay sang time_t: " + ngay, LOI);
    }
    return result;
}

int tinhSoNgayQuaHan(const std::string& ngayMuon, std::ostream& out) {
    time_t now = time(nullptr); // Lay thoi gian hien tai
    time_t tMuon = ChuyenNgaySangTimeT(ngayMuon, out); // Chuyen ngay muon sang time_t
    if (tMuon == -1) return -1; // Loi chuyen doi ngay muon

    // Tinh so giay chenh lech giua hien tai va ngay muon
    double diff_seconds = difftime(now, tMuon);
    // Chuyen so giay sang so ngay (chia cho 60*60*24)
    double diff_days = diff_seconds / (60 * 60 * 24);

    // Tinh so ngay da qua han (so ngay muon - so ngay toi da)
    int soNgayQuaHan = static_cast<int>(diff_days) - SO_NGAY_MUON_TOI_DA;

    // Neu so ngay qua han < 0 thi tra ve 0 (chua qua han)
    return soNgayQuaHan > 0 ? soNgayQuaHan : 0;
}

// Phân tích chuỗi ngày "dd/mm/yyyy" thanh cac so nguyen
bool phanTichNgay(const std::string& ngay, int& d, int& m, int& y) {
    // Kiem tra do dai va dinh dang co ban (co dau '/')
    if (ngay.length() != 10 || ngay[2] != '/' || ngay[5] != '/') return false;
    try {
        // Su dung std::stoi de chuyen doi chuoi con sang so nguyen
        d = std::stoi(ngay.substr(0, 2));
        m = std::stoi(ngay.substr(3, 2));
        y = std::stoi(ngay.substr(6, 4));
        // Kiem tra gia tri ngay thang nam co hop le khong
        if (d < 1 || d > 31 || m < 1 || m > 12 || y < NAM_XUAT_BAN_MIN) return false;
        // Chua kiem tra ngay cuoi thang (vd: 31/02), mktime se xu ly sau
        return true;
    } catch (...) {
        // Bat loi neu std::stoi khong chuyen doi duoc (vd: "aa/bb/cccc")
        return false;
    }
}