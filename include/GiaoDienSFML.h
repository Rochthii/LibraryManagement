#ifndef GIAODIEN_SFML_H
#define GIAODIEN_SFML_H

#include <SFML/Graphics.hpp>
#include <string>

// --- Kích Thước & Màu Sắc ---
const unsigned int CHIEU_RONG = 1024;
const unsigned int CHIEU_CAO = 768;

const sf::Color MAU_NEN(30, 30, 45);
const sf::Color MAU_TIEU_DE(255, 200, 100);
const sf::Color MAU_NEN_NUT(100, 100, 110);
const sf::Color MAU_CHU_NUT(220, 220, 220);
const sf::Color MAU_CHU(200, 200, 200);
const sf::Color MAU_NHAN(100, 150, 255);
const sf::Color MAU_NUT_BACK(80, 80, 90);

// --- Trạng Thái Màn Hình ---
enum TrangThaiManHinh {
    MENU_CHINH,
    MAN_HINH_THONG_TIN,
    QUAN_LY_SACH // Dùng sau
};

// --- Mã Định Danh Nút Bấm ---
enum MaNut {
    NUT_QUAN_LY_DAU_SACH,
    NUT_QUAN_LY_DOC_GIA,
    NUT_MUON_TRA_SACH,
    NUT_THONG_TIN,
    NUT_THOAT,
    NUT_BACK,
    NUT_KHONG_XAC_DINH
};

// --- Cấu trúc nút bấm SFML ---
struct NutBam {
    sf::RectangleShape hinhDang;
    sf::Text vanBan;
    MaNut id;
};


#define SO_NUT_TOI_DA 10

bool KhoiTaoGiaoDienSFML(sf::RenderWindow& window, sf::Font& font, const std::string& fontPath);
void ChayChuongTrinhSFML(sf::RenderWindow& window, sf::Font& font);

#endif // GIAODIEN_SFML_H