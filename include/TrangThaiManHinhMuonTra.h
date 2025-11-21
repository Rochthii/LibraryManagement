// [FILE: TrangThaiManHinhMuonTra.h]
#ifndef TRANG_THAI_MAN_HINH_MUON_TRA_H
#define TRANG_THAI_MAN_HINH_MUON_TRA_H

#include "DocGia.h"
#include "QuanLySach.h"
#include <SFML/Graphics.hpp>
#include <string>

// Các trạng thái chính của màn hình
enum CheDoMuonTra {
    MT_HOME,            // Màn hình chọn: [Mượn/Trả] hoặc [Top 10]
    MT_TOP_10,          // Màn hình xem Top 10
    MT_PROCESS          // Màn hình xử lý chính (Dock UI)
};

// Các bước trong quy trình Mượn/Trả (khi ở MT_PROCESS)
enum QuyTrinhCon {
    BUOC_CHON_DOC_GIA,  // Dock trái: List ĐG, Dock phải: Trống/Hdẫn
    BUOC_CHON_HANH_DONG,// Dock trái: Highlight ĐG, Dock phải: 3 nút Xem/Mượn/Trả
    BUOC_XEM_SACH,      // Dock trái: Info ĐG, Dock phải: List sách đang mượn
    BUOC_MUON_SACH,     // Dock trái: List Sách (Tìm), Dock phải: Info sách chọn + Nút Mượn
    BUOC_TRA_SACH       // Dock trái: List Sách đang mượn, Dock phải: Info sách chọn + Nút Trả
};

struct MuonTraState {
    CheDoMuonTra cheDo;
    QuyTrinhCon buocHienTai;

    // --- Dữ liệu Độc Giả ---
    std::string tuKhoaTimDG;
    PTRDG mangDocGiaHienThi[MAX_DAUSACH]; // Danh sách hiển thị ở Dock Trái
    int soLuongDGHienThi;
    PTRDG docGiaDangChon;                 // Độc giả đang được thao tác
    
    // --- Dữ liệu Sách (Khi Mượn) ---
    std::string tuKhoaTimSach;
    KetQuaTimKiem ketQuaTimSach[MAX_KET_QUA_TIM_KIEM];
    int soLuongSachTimThay;
    PTRDS dauSachDangChon;                // Đầu sách chọn để mượn
    
    // --- Dữ liệu Mượn/Trả ---
    ThongTinSachDangMuon_DTO listSachDangMuon[10]; // Cache sách đang mượn của ĐG
    int slSachDangMuon;
    std::string maSachCanTra;             // Mã cá biệt chọn để trả
    int indexSachTra;                     // Index trong mảng listSachDangMuon

    // --- Top 10 ---
    TopSachDTO top10[10];
    int soLuongTop;

    // --- Chung ---
    int trangHienTai;   // Dùng cho list ở Dock Trái
    int tongSoTrang;
    
    // --- Modal ---
    bool hienModal;
    std::string noiDungModal;
    bool isConfirmAction; // True: XN Mượn/Trả, False: Thông báo

    MuonTraState() { Reset(); }

    void Reset() {
        cheDo = MT_HOME;
        buocHienTai = BUOC_CHON_DOC_GIA;
        docGiaDangChon = nullptr;
        dauSachDangChon = nullptr;
        tuKhoaTimDG = "";
        tuKhoaTimSach = "";
        maSachCanTra = "";
        indexSachTra = -1;
        trangHienTai = 1;
        tongSoTrang = 1;
        hienModal = false;
    }
};

// API
void KhoiTaoManHinhMuonTra();
void VeManHinhMuonTra(sf::RenderWindow &window, const sf::Font &font);
void XuLySuKienManHinhMuonTra(sf::RenderWindow &window, sf::Event event);

#endif