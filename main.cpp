#include <iostream>
#include <filesystem> 
#include <SFML/Graphics.hpp>
#include <string>
#include "include/QuanLySach.h"
#include "include/ThaoTacFile.h"
#include "include/ThongBao.h"
#include "include/GiaoDienSFML.h"

// bien toan cuc
extern PTRDS dsDauSach[MAX_DAUSACH];
extern int soLuongDauSach;
extern bool duLieuDaThayDoi;

int main() {
    std::cout << "[main] Bat dau chuong trinh..." << std::endl;

    // b1: Nap du lieu tu file 
    std::cout << "Dang nap du lieu..." << std::endl;
    // nap dau sach
    NapDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach, std::cout);
    // napp danh muc sanh
    NapDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach, std::cout);

    if (soLuongDauSach > 0) {
        std::cout << "Nap du lieu thanh cong. So luong dau sach: " << soLuongDauSach << std::endl;
        sapXepDauSachTheoTen(dsDauSach, 0, soLuongDauSach - 1);
    }
    else {
        std::cout << "Khong nap duoc dau sach nao hoac file du lieu rong/loi." << std::endl;
    }

    // b2:Khoi tao SFML
    std::cout << "Dang khoi tao SFML..." << std::endl;
    sf::RenderWindow window;
    sf::Font font;

    // tao cua so SFML
    window.create(sf::VideoMode(CHIEU_RONG, CHIEU_CAO), "Quan Ly Thu Vien (SFML)");
    window.setFramerateLimit(60);

    // tim font tu dong
    const char *candidates[] = {"bin/DejaVuSans.ttf", "files/DejaVuSans.ttf", "arial.ttf", "C:/Windows/Fonts/arial.ttf"};
    std::string fontPath;
    for (auto c : candidates)
        if (std::filesystem::exists(c)) {
            fontPath = c;
            break;
        }
    if (fontPath.empty()) {
        std::cerr << "Thieu font (dat DejaVuSans.ttf hoac arial.ttf vao thu muc files/ hoac bin/)\n";
        // giai phong bo nho da nap truoc khi thoat loi
        GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach); //
        return 1;
    }
    std::cout << "Tim thay font tai: " << fontPath << std::endl;

    // goi ham khoi tao giao dien (chu yeu tai font)
    if (!KhoiTaoGiaoDienSFML(window, font, fontPath)) {
        std::cerr << "Loi khoi tao giao dien SFML!" << std::endl;
        GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach); //// giai phong bo nho da nap truoc khi thoat loi
        return 2;
    }

    // b3: Chay vong lap chinh cua giao dien 
    std::cout << "Bat dau vong lap chuong trinh SFML..." << std::endl;
    ChayChuongTrinhSFML(window, font); 
    std::cout << "Da thoat khoi vong lap chuong trinh SFML." << std::endl;

    // b4: Luu file va giai phong bo nho
    if (duLieuDaThayDoi){                                                          
        thongBao(std::cout, "Dang luu thay doi...", THONG_TIN); //
        // goi ham ghi file dau sach
        GhiDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach, false, std::cout);
        // goi ham ghi file danh muc sach
        GhiDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach, false, std::cout);
    }
    else {
        std::cout << "Khong co thay doi du lieu de luu." << std::endl;
    }

    // giai phong toan bo bo nho da cap phat cho danh sach
    GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
    std::cout << "Da giai phong bo nho." << std::endl;

    std::cout << "[main] Ket thuc chuong trinh." << std::endl;
    return 0;
}
