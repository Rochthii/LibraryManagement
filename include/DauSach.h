#ifndef DAUSACH_H
#define DAUSACH_H

#include <string>
#include <iostream>
#include "Constants.h"

// Dinh nghia trang thai cua mot cuon sach cu the (ban sao)
enum TrangThaiSach {
    CHO_MUON_DUOC = 0,  // co the muon
    DANG_MUON = 1,      // dg co doc gia muon
    THANH_LY = 2        // Sach da hong, mat, hoac het han su dung
};

// Cau truc node cho Danh Muc Sach (DSLK don cac ban sao cua mot Dau Sach)
struct DanhMucSach {
    std::string maSach;         // Ma duy nhat cho tung cuon (ISBN-SoThuTu)
    TrangThaiSach trangThai;    // 0, 1, 2 theo enum TrangThaiSach
    std::string viTri;          // Vi tri cu the tren ke sach (vd: "Ke A1")
    DanhMucSach* next = NULL;   // Con tro toi node ban sao tiep theo
};

typedef DanhMucSach* PTRDMS;    // Con tro toi mot node DanhMucSach

// Cau truc Dau Sach (thong tin chung cua mot tua sach)
struct DauSach {
    std::string ISBN;           // Ma so sach quoc te (duy nhat cho moi Dau Sach)
    std::string tenSach;
    int soTrang;
    std::string tacGia;
    int namXuatBan;
    std::string theLoai;
    PTRDMS dms = NULL;          // Con tro toi dau danh sach lien ket cac ban sao (DanhMucSach)
    int tongBanSao;             // Tong so luong ban sao hien co (cap nhat khi can)
};

typedef DauSach* PTRDS;         // Con tro toi mot DauSach

// Bien toan cuc (khai bao extern de cac file khac co the truy cap)
extern PTRDS dsDauSach[MAX_DAUSACH]; // Mang con tro luu tru cac Dau Sach
extern int soLuongDauSach;           // So luong Dau Sach hien co trong mang
extern bool duLieuDaThayDoi;         // Co de kiem tra viec luu file khi thoat

#endif // DAUSACH_H