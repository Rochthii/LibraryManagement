/*
#include "include/GiaoDien.h"
#include <iostream>
#include <cstdlib>
#include <conio.h>

// --- Bien Trang Thai ---
// static TrangThaiManHinh manHinhHienTai = MENU_CHINH; // Bat dau voi Menu - Sua lai duoi day
static TrangThaiManHinh manHinhHienTai = MENU_CHINH; // Đảm bảo bắt đầu với Menu
static PhanTuNhanMenu cacVungNhanMenu[SO_NUT_MENU_TOI_DA];
static int soLuongVungNhanMenu = 0;
static bool yeuCauThoat = false; // Co de thoat vong lap chinh

// --- Khai Bao Ham Noi Bo ---
static void XuLySuKienMenu();
static MaNutMenu LayMaNutMenuTaiToaDo(int x, int y);
static void LuuVungNhanMenu(MaNutMenu id, int x1, int y1, int x2, int y2);
// Khai báo các hàm vẽ màn hình khác nếu cần
void VeTabQuanLySach(); // Khai báo tạm để tránh lỗi khi gọi trong switch

// --- Ham Ve Co Ban ---

static void LuuVungNhanMenu(MaNutMenu id, int x1, int y1, int x2, int y2) {
    if (soLuongVungNhanMenu < SO_NUT_MENU_TOI_DA) {
        cacVungNhanMenu[soLuongVungNhanMenu].id = id;
        cacVungNhanMenu[soLuongVungNhanMenu].vung = {x1, y1, x2, y2};
        soLuongVungNhanMenu++;
    }
}

void VeNutMenu(int x, int y, int rong, int cao, const char* nhan, int mauNen, int mauChu) {
    setfillstyle(SOLID_FILL, mauNen);
    bar(x, y, x + rong, y + cao);
    setbkcolor(mauNen);
    setcolor(mauChu);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(x + rong / 2, y + cao / 2, (char*)nhan);
    // Reset ve mac dinh
    settextjustify(LEFT_TEXT, TOP_TEXT);
    setbkcolor(MAU_NEN);
    setcolor(MAU_CHU);
}

// --- Ve Cac Man Hinh ---

void VeMenuChinh() {
    // Tieu de lon o giua
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 4); // Size 4
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    setcolor(MAU_TIEU_DE);
    outtextxy(CHIEU_RONG / 2, 150, (char*)"QUAN LY THU VIEN");

    // Ve cac nut bam
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1); // Reset size
    int nutRong = 300;
    int nutCao = 50;
    int khoangCachNut = 20;
    int x = (CHIEU_RONG - nutRong) / 2;
    int y = 250;

    // Ve cac nut va luu vung nhan
    VeNutMenu(x, y, nutRong, nutCao, (char*)"QUAN LY DAU SACH", MAU_NEN_NUT, MAU_CHU_NUT);
    LuuVungNhanMenu(NUT_QUAN_LY_DAU_SACH, x, y, x + nutRong, y + nutCao);
    y += nutCao + khoangCachNut;

    VeNutMenu(x, y, nutRong, nutCao, (char*)"QUAN LY DOC GIA", MAU_NEN_NUT, MAU_CHU_NUT);
    LuuVungNhanMenu(NUT_QUAN_LY_DOC_GIA, x, y, x + nutRong, y + nutCao);
    y += nutCao + khoangCachNut;

    VeNutMenu(x, y, nutRong, nutCao, (char*)"MUON TRA SACH", MAU_NEN_NUT, MAU_CHU_NUT);
    LuuVungNhanMenu(NUT_MUON_TRA_SACH, x, y, x + nutRong, y + nutCao);
    y += nutCao + khoangCachNut;

    VeNutMenu(x, y, nutRong, nutCao, (char*)"THONG TIN", MAU_NEN_NUT, MAU_CHU_NUT);
    LuuVungNhanMenu(NUT_THONG_TIN, x, y, x + nutRong, y + nutCao);
    y += nutCao + khoangCachNut;

    VeNutMenu(x, y, nutRong, nutCao, (char*)"THOAT", MAU_NEN_NUT, MAU_CHU_NUT);
    LuuVungNhanMenu(NUT_THOAT, x, y, x + nutRong, y + nutCao);
}

void VeManHinhThongTin() {
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    setbkcolor(MAU_NEN);

    int centerX = CHIEU_RONG / 2;
    int currentY = 150;

    setcolor(MAU_TIEU_DE);
    outtextxy(centerX, currentY, (char*)"Hoc vien Cong nghe Buu Chinh Vien Thong - TPHCM");
    currentY += 80;

    settextjustify(LEFT_TEXT, TOP_TEXT);
    setcolor(MAU_CHU);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    int labelX = centerX - 300;
    int valueX = centerX - 150;

    outtextxy(labelX, currentY, (char*)"* Mon hoc:");
    setcolor(MAU_NHAN);
    outtextxy(valueX, currentY, (char*)"Cau Truc Du Lieu & Giai Thuat");
    currentY += 40;
    setcolor(MAU_CHU);
    outtextxy(labelX, currentY, (char*)"* Giang vien:");
    setcolor(MAU_NHAN);
    outtextxy(valueX, currentY, (char*)"Luu Nguyen Ky Thu");
    currentY += 40;
    setcolor(MAU_CHU);
    outtextxy(labelX, currentY, (char*)"* Do An:");
    setcolor(MAU_NHAN);
    outtextxy(valueX, currentY, (char*)"Quan Ly Thu Vien");
    currentY += 80;

    setcolor(MAU_CHU);
    outtextxy(labelX, currentY, (char*)"* Sinh vien:");
    setcolor(MAU_NHAN);
    outtextxy(valueX, currentY, (char*)"1. Cham Roch Thi - N24DECE041 - E24CQCE01-N");
    currentY += 30;
    outtextxy(valueX, currentY, (char*)"2. Tran Gia Binh - N24DECE05 - E24CQCE01-N");
    currentY += 100;

    setcolor(MAU_CHU);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    outtextxy(centerX, currentY, (char*)"-- 2025 --");
    settextjustify(LEFT_TEXT, TOP_TEXT);

    VeNutMenu(20, 20, 100, 40, (char*)"< BACK", COLOR(80, 80, 90), MAU_CHU);
}

// --- Xu Ly Su Kien ---

static MaNutMenu LayMaNutMenuTaiToaDo(int x, int y) {
    for (int i = 0; i < soLuongVungNhanMenu; ++i) {
        VungNhan vung = cacVungNhanMenu[i].vung;
        if (x >= vung.x1 && x <= vung.x2 && y >= vung.y1 && y <= vung.y2) {
            return cacVungNhanMenu[i].id;
        }
    }
    return NUT_MENU_KHONG_XAC_DINH;
}

static void XuLySuKienMenu() {
    if (ismouseclick(WM_LBUTTONDOWN)) {
        int x, y;
        getmouseclick(WM_LBUTTONDOWN, x, y);
        clearmouseclick(WM_LBUTTONDOWN);

        MaNutMenu idNut = LayMaNutMenuTaiToaDo(x, y);

        switch (idNut) {
            case NUT_QUAN_LY_DAU_SACH:
                manHinhHienTai = QUAN_LY_SACH;
                std::cout << "[DEBUG] Chuyen sang Quan Ly Sach (Chua lam)\n";
                break;
            case NUT_THONG_TIN:
                // *** SUA LOI: Su dung MAN_HINH_THONG_TIN ***
                manHinhHienTai = MAN_HINH_THONG_TIN;
                break;
            case NUT_THOAT:
                yeuCauThoat = true;
                break;
            case NUT_QUAN_LY_DOC_GIA:
            case NUT_MUON_TRA_SACH:
                 std::cout << "[DEBUG] Chuc nang chua duoc thuc hien!\n";
                 break;
            default:
                break;
        }
    }
}

// --- Ham Public Chinh ---

void KhoiTaoGiaoDien() {
    std::cout << "Dang khoi tao cua so do hoa..." << std::endl;
    initwindow(CHIEU_RONG, CHIEU_CAO, (char*)"DO AN DSA: QUAN LY THU VIEN");

    int errorcode = graphresult();
    if (errorcode != grOk) {
        std::cout << "Loi khoi tao do hoa: " << grapherrormsg(errorcode) << std::endl;
        std::cout << "Ma loi: " << errorcode << std::endl;
        std::cout << "Nhan phim bat ky de thoat..." << std::endl;
        getch();
        exit(1);
    }
    std::cout << "Khoi tao do hoa thanh cong!" << std::endl;
    setbkcolor(MAU_NEN);
}

void DongGiaoDien() {
    closegraph();
}

// *** SUA LOI: Khai bao VeTabQuanLySach de tranh loi bien dich ***
// (Ban co the dat phan dinh nghia nay o duoi cung file neu muon)
void VeTabQuanLySach() { // Ham tam thoi
    setcolor(WHITE);
    outtextxy(100, 100, (char*)"Man hinh Quan Ly Sach (Chua lam)");
}


void ChayChuongTrinh() {
    int page = 0;
    yeuCauThoat = false;

    while (!yeuCauThoat) {
        setactivepage(page);
        cleardevice();

        if(manHinhHienTai == MENU_CHINH) {
            soLuongVungNhanMenu = 0;
        }

        switch (manHinhHienTai) {
            case MENU_CHINH:
                VeMenuChinh();
                break;
            // *** SUA LOI: Su dung MAN_HINH_THONG_TIN ***
            case MAN_HINH_THONG_TIN:
                VeManHinhThongTin();
                break;
            case QUAN_LY_SACH:
                 VeTabQuanLySach(); // Goi ham tam thoi
                 break;
        }

        setvisualpage(page);
        page = 1 - page;

        switch (manHinhHienTai) {
            case MENU_CHINH:
                XuLySuKienMenu();
                break;
            // *** SUA LOI: Su dung MAN_HINH_THONG_TIN ***
            case MAN_HINH_THONG_TIN:
                if (kbhit()) {
                    char key = getch();
                    if (key == 27) {
                        manHinhHienTai = MENU_CHINH;
                    }
                }
                 if (ismouseclick(WM_LBUTTONDOWN)) {
                     int x, y;
                     getmouseclick(WM_LBUTTONDOWN, x, y);
                     clearmouseclick(WM_LBUTTONDOWN);
                     if (x >= 20 && x <= 120 && y >= 20 && y <= 60) {
                         manHinhHienTai = MENU_CHINH;
                     }
                 }
                break;
            case QUAN_LY_SACH:
                  if (kbhit()) {
                    char key = getch();
                    if (key == 27) {
                        manHinhHienTai = MENU_CHINH;
                    }
                }
                 break;
        }

        if (manHinhHienTai != MENU_CHINH && GetAsyncKeyState(VK_ESCAPE)) {
             manHinhHienTai = MENU_CHINH;
             delay(100);
        }

        delay(20);
    }
     std::cout << "Da yeu cau thoat. Dong cua so...\n";
}

*/