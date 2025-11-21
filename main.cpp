#include <iostream>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "QuanLySach.h"
#include "ThaoTacFile.h"
#include "ThongBao.h"
#include "GiaoDienSFML.h"
#include "DocGia.h"     // [THÊM VÀO 1]
#include "NgayThang.h"  // [THÊM VÀO 2] (Cần cho các hàm của DocGia)

// Biến toàn cục cho Sách
extern PTRDS dsDauSach[MAX_DAUSACH];
extern int soLuongDauSach;
extern bool duLieuDaThayDoi;

// [THÊM VÀO 3] Định nghĩa biến toàn cục cho Độc Giả (Sửa lỗi Linker)
PTRDG rootDocGia = nullptr;

int main() {
    std::cout << "[main] Bat dau...\n";

    // --- NẠP DỮ LIỆU ---
    // (Thứ tự nạp rất quan trọng)
    std::cout << "Dang nap du lieu Dau Sach...\n";
    NapDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach, std::cout);
    NapDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach, std::cout);

    if (soLuongDauSach > 0) {
        std::cout << "Nap thanh cong: " << soLuongDauSach << " dau sach.\n";
        sapXepDauSachTheoTen(dsDauSach, 0, soLuongDauSach - 1); // sap xep theo ten
    } else {
        std::cout << "Khong nap duoc dau sach nao.\n";
    }

    // [THÊM VÀO 4] Nạp dữ liệu Độc Giả (PHẢI nạp sau khi nạp Sách)
    std::cout << "Dang nap du lieu Doc Gia...\n";
    rootDocGia = loadDocGia(dsDauSach, soLuongDauSach);
    if (rootDocGia != nullptr) {
        std::cout << "Nap thanh cong du lieu Doc Gia.\n";
    } else {
        std::cout << "Khong nap duoc du lieu Doc Gia (file co thể rong).\n";
    }

    // --- KHỞI TẠO CỬA SỔ ---
    sf::RenderWindow window(sf::VideoMode(CHIEU_RONG, CHIEU_CAO), "Quan Ly Thu Vien");
    window.setFramerateLimit(60);
    sf::Font font;

    // Tim font tu dong
    const char* fonts[] = {"bin/DejaVuSans.ttf", "files/DejaVuSans.ttf", "arial.ttf", "C:/Windows/Fonts/arial.ttf"};
    std::string fontPath;
    for (auto f : fonts) if (std::filesystem::exists(f)) { fontPath = f; break; }

    if (fontPath.empty() || !font.loadFromFile(fontPath)) { // loi font
        std::cerr << "Loi: Khong tim/load duoc font!\n";
        GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
        GiaiPhongCay(rootDocGia); // [THÊM VÀO 5] (Giải phóng nếu lỗi)
        return 1;
    }

    if (!KhoiTaoGiaoDienSFML(window, font, fontPath)) { // loi giao dien
        std::cerr << "Loi khoi tao giao dien!\n";
        GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
        GiaiPhongCay(rootDocGia); // [THÊM VÀO 6] (Giải phóng nếu lỗi)
        return 2;
    }

    // --- CHẠY VÒNG LẶP CHÍNH ---
    // (Hàm này sẽ gọi VeManHinh... và XuLySuKien... cho DocGia và MuonTra)
    ChayChuongTrinhSFML(window, font);

    // --- LƯU TRƯỚC KHI THOÁT ---
    if (duLieuDaThayDoi) {
        thongBao(std::cout, "Dang luu thay doi...", THONG_TIN);
        // Lưu Sách
        GhiDanhSachDauSach(FILE_DAUSACH, dsDauSach, soLuongDauSach, false, std::cout);
        GhiDanhMucSach(FILE_DANHMUCSACH, dsDauSach, soLuongDauSach, false, std::cout);
        // [THÊM VÀO 7] Lưu Độc Giả
        saveDocGia(rootDocGia);
        std::cout << "Luu du lieu Doc Gia thanh cong.\n";
    } else {
        std::cout << "Khong co thay doi.\n";
    }

    // --- GIẢI PHÓNG BỘ NHỚ ---
    std::cout << "Dang giai phong bo nho...\n";
    GiaiPhongToanBoDauSach(dsDauSach, soLuongDauSach);
    GiaiPhongCay(rootDocGia); // [THÊM VÀO 8]
    std::cout << "[main] Ket thuc.\n";
    return 0;
}