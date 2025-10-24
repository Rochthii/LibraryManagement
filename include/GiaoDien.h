/*

#ifndef GIAODIEN_H
#define GIAODIEN_H

#include "graphics.h"
#include <string>

// --- Dinh Nghia Mau Sac ---
#define MAU_NEN COLOR(30, 30, 45)
#define MAU_TIEU_DE COLOR(255, 200, 100)
#define MAU_NEN_NUT LIGHTGRAY // Hoac COLOR(100, 100, 110)
#define MAU_CHU_NUT BLACK     // Hoac COLOR(220, 220, 220) neu nen toi
#define MAU_CHU COLOR(200, 200, 200)
#define MAU_NHAN COLOR(100, 150, 255)

// --- Kich Thuoc ---
#define CHIEU_RONG 1280
#define CHIEU_CAO 720

// --- Trang Thai Man Hinh ---
enum TrangThaiManHinh {
    MENU_CHINH,
    MAN_HINH_THONG_TIN,
    QUAN_LY_SACH // Se them sau
    // Them cac man hinh khac o day
};

// --- Ma Dinh Danh Nut Bam (Cho Menu) ---
enum MaNutMenu {
    NUT_QUAN_LY_DAU_SACH,
    NUT_QUAN_LY_DOC_GIA, // Chua lam
    NUT_MUON_TRA_SACH,   // Chua lam
    NUT_THONG_TIN,
    NUT_THOAT,
    NUT_MENU_KHONG_XAC_DINH
};

// --- Vung Nhan (Hitbox) ---
struct VungNhan {
    int x1, y1, x2, y2;
};

struct PhanTuNhanMenu {
    MaNutMenu id;
    VungNhan vung;
};
#define SO_NUT_MENU_TOI_DA 10 // Du cho menu hien tai

// =================================================================
// Khai Bao Ham Giao Dien
// =================================================================

void KhoiTaoGiaoDien();
void DongGiaoDien();
void ChayChuongTrinh();
void VeMenuChinh();
void VeManHinhThongTin();
void VeNutMenu(int x, int y, int rong, int cao, const char* nhan, int mauNen, int mauChu);

#endif // GIAODIEN_H

*/