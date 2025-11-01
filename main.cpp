#include <iostream>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "include/QuanLySach.h"
#include "include/ThaoTacFile.h"
#include "include/ThongBao.h"
#include "include/GiaoDienSFML.h"

extern PTRDS dsDauSach[MAX_DAUSACH];
extern int soLuongDauSach;
extern bool duLieuDaThayDoi;

int main() {
    std::cout << "[main] Bat dau...\n";

    // nap du lieu tu file
    NapDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach, std::cout);
    NapDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach, std::cout);

    if (soLuongDauSach > 0) {
        std::cout << "Nap thanh cong: " << soLuongDauSach << " dau sach.\n";
        sapXepDauSachTheoTen(dsDauSach, 0, soLuongDauSach - 1);                    // sap xep theo ten
    } else {
        std::cout << "Khong nap duoc dau sach nao.\n";
    }

    // tao cua so sfml
    sf::RenderWindow window(sf::VideoMode(CHIEU_RONG, CHIEU_CAO), "Quan Ly Thu Vien");
    window.setFramerateLimit(60);
    sf::Font font;

    // tim font tu dong
    const char* fonts[] = {"bin/DejaVuSans.ttf", "files/DejaVuSans.ttf", "arial.ttf", "C:/Windows/Fonts/arial.ttf"};
    std::string fontPath;
    for (auto f : fonts) if (std::filesystem::exists(f)) { fontPath = f; break; }

    if (fontPath.empty() || !font.loadFromFile(fontPath)) {                         // loi font
        std::cerr << "Loi: Khong tim/load duoc font!\n";
        GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
        return 1;
    }

    if (!KhoiTaoGiaoDienSFML(window, font, fontPath)) {                             // loi giao dien
        std::cerr << "Loi khoi tao giao dien!\n";
        GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
        return 2;
    }

    // chay vong lap chinh
    ChayChuongTrinhSFML(window, font);

    // luu neu co thay doi
    if (duLieuDaThayDoi) {
        thongBao(std::cout, "Dang luu thay doi...", THONG_TIN);
        GhiDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach, false, std::cout);
        GhiDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach, false, std::cout);
    } else {
        std::cout << "Khong co thay doi.\n";
    }

    // giai phong bo nho
    GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
    std::cout << "[main] Ket thuc.\n";
    return 0;
}