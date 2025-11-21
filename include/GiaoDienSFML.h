#ifndef GIAODIEN_SFML_H
#define GIAODIEN_SFML_H

#include <SFML/Graphics.hpp>
#include <string>
#include "QuanLySach.h" 

// =============================================================
// 1. HẰNG SỐ KÍCH THƯỚC & MÀU SẮC (GIỮ NGUYÊN CŨ)
// =============================================================

const unsigned int CHIEU_RONG = 1600;
const unsigned int CHIEU_CAO = 800;

const sf::Color MAU_NEN(30, 30, 45);
const sf::Color MAU_TIEU_DE(255, 200, 100);
const sf::Color MAU_NEN_NUT(100, 100, 110);
const sf::Color MAU_CHU_NUT(220, 220, 220);
const sf::Color MAU_CHU(200, 200, 200);
const sf::Color MAU_NHAN(100, 150, 255);
const sf::Color MAU_NUT_BACK(80, 80, 90);
const sf::Color MAU_KHUNG(50, 50, 65);
const sf::Color MAU_VIEN(150, 150, 160);
const sf::Color MAU_INPUT_NEN(60, 60, 75);
const sf::Color MAU_INPUT_CHU(210, 210, 210);
const sf::Color MAU_INPUT_BORDER(120, 120, 130);
const sf::Color MAU_INPUT_ACTIVE_BORDER(100, 150, 255);
const sf::Color MAU_LOI(255, 100, 100);
const sf::Color MAU_THANH_CONG(100, 255, 100);
const sf::Color MAU_BANG_HEADER(45, 45, 60);
const sf::Color MAU_BANG_BORDER(80, 80, 90);

const unsigned int FONT_SIZE_NHO = 16;
const unsigned int FONT_SIZE_BINH_THUONG = 18;
const unsigned int FONT_SIZE_TIEU_DE_KHUNG = 22;
const unsigned int FONT_SIZE_TIEU_DE_LON = 32;
const unsigned int FONT_SIZE_TIEU_DE_MAIN = 42;

const float THANH_TAB_CAO = 40.f;
const float PADDING = 20.f;
const float FORM_RONG = 450.f;
const float BANG_X = PADDING;
const float BANG_Y = THANH_TAB_CAO + PADDING;
const float BANG_RONG = CHIEU_RONG - FORM_RONG - 3 * PADDING;
const float FORM_X = BANG_X + BANG_RONG + PADDING;
const float FORM_Y = BANG_Y + 50.f;
const float KHUNG_THONG_BAO_Y = CHIEU_CAO - PADDING - 140.f;
const float KHUNG_THONG_BAO_CAO = 140.f;
const float NUT_CAO = 40.f;
const float INPUT_CAO = 40.f;
const float INPUT_RONG = FORM_RONG - 2 * PADDING - 110.f;
const int SACH_MOI_TRANG = 17;

// =============================================================
// 2. ENUM TRẠNG THÁI & ID UI (HỢP NHẤT TẤT CẢ)
// =============================================================

enum TrangThaiManHinh {
<<<<<<< HEAD
    MENU_CHINH,         // man hinh menu ban dau
    MAN_HINH_THONG_TIN, // man hinh gioi thieu thong tin
    QUAN_LY_SACH        // man hinh quan ly dau sach
    //co the them MAN_HINH_DOC_GIA, MAN_HINH_MUON_TRA,...
    QUAN_LY_DOC_GIA    // Mới
=======
    MENU_CHINH,
    MAN_HINH_THONG_TIN,
    QUAN_LY_SACH,
    QUAN_LY_DOC_GIA,    // Mới
>>>>>>> eaa30afcf8f938ebcb2bae94690842f583f44ad1
    MUON_TRA_SACH       // Mới
};

enum MaUI {
    // --- CHUNG ---
    KHONG_XAC_DINH,
    NUT_BACK,           // Nút quay lại
    HANG_SACH,          // Click vào dòng trong bảng

    // --- MENU CHÍNH ---
    NUT_QUAN_LY_DAU_SACH,
    NUT_QUAN_LY_DOC_GIA,
    NUT_MUON_TRA_SACH,
    NUT_THONG_TIN,
    NUT_THOAT,

    // --- QUẢN LÝ SÁCH (CŨ & BỔ SUNG THIẾU) ---
    INPUT_TIM_SACH, NUT_TIM, NUT_XOA_TIM,
    NUT_CHUYEN_CHE_DO_XEM,
    INPUT_ISBN, INPUT_TEN_SACH, INPUT_SO_TRANG, 
    INPUT_TAC_GIA, INPUT_NAM_XB, INPUT_THE_LOAI, INPUT_SO_LUONG,
    INPUT_VI_TRI,          // <--- Bổ sung ID bị thiếu
    NUT_THEM_SACH, NUT_SUA, NUT_XOA, 
    NUT_XAC_NHAN_XOA, NUT_HUY_XOA,
    NUT_TRANG_TRUOC, NUT_TRANG_SAU, NUT_HUY_CHON,
    NUT_MODAL_TRANG_TRUOC, NUT_MODAL_TRANG_SAU,

    // Modal chi tiết bản sao
    NUT_CHI_TIET_SACH, NUT_DONG_MODAL_CHITIET,
    NUT_XAC_NHAN_THANHLY, NUT_HUY_THANHLY,
    NUT_XAC_NHAN_XOA_BANSAO, NUT_HUY_XOA_BANSAO, // <--- Bổ sung ID bị thiếu

    // Modal thêm bản sao
    NUT_THEM_BAN_SAO, INPUT_SO_LUONG_THEM, 
    NUT_XAC_NHAN_THEM_BS, NUT_HUY_THEM_BS,

    // --- QUẢN LÝ ĐỘC GIẢ (MỚI) ---
    NUT_XEM_THEO_MA, NUT_XEM_THEO_TEN, NUT_XEM_QUA_HAN,
    INPUT_HO_DOC_GIA, INPUT_TEN_DOC_GIA,
    NUT_THEM_DOC_GIA, NUT_LUU_DOC_GIA, NUT_HUY_DOC_GIA,
    NUT_CHI_TIET_DOC_GIA, NUT_SUA_DOC_GIA, NUT_XOA_DOC_GIA, NUT_HUY_CHON_DOC_GIA,
    NUT_TRANG_TRUOC_DG, NUT_TRANG_SAU_DG,
    NUT_XAC_NHAN_XOA_DG, NUT_HUY_XOA_DG,
    NUT_DONG_MODAL_DG,

    // --- MƯỢN TRẢ SÁCH (MỚI) ---
    NUT_MT_VAO_MUON_TRA, NUT_MT_VAO_TOP_10,
    
    // Dock Trái
    INPUT_MT_TIM_DOC_GIA, INPUT_MT_TIM_SACH,
    NUT_MT_TIM_DG_BTN, NUT_MT_TIM_SACH_BTN,
    NUT_MT_PREV_PAGE, NUT_MT_NEXT_PAGE,

    // Dock Phải
    NUT_MT_HANHDONG_XEM, NUT_MT_HANHDONG_MUON, NUT_MT_HANHDONG_TRA,
    NUT_MT_XAC_NHAN_MUON, NUT_MT_XAC_NHAN_TRA, NUT_MT_HUY_HANH_DONG,
    
    // Modal xác nhận Mượn/Trả
    NUT_MT_XAC_NHAN_MODAL, NUT_MT_HUY_MODAL
};

// =============================================================
// 3. STRUCT & GLOBAL VARS (GIỮ NGUYÊN)
// =============================================================

struct UIElement {
    sf::RectangleShape hinhDang;
    sf::Text vanBan;
    sf::Text vanBanPhu;
    MaUI id;
    bool laInput = false;
};

#define SO_ELEMENT_TOI_DA 100 // Tăng lên 100 cho an toàn vì nhiều nút hơn

extern TrangThaiManHinh manHinhHienTai;
extern UIElement cacElement[SO_ELEMENT_TOI_DA];
extern int soLuongElement;
extern bool yeuCauThoat;
extern MaUI inputHoatDong;
extern MaUI elementHover;
extern std::string noiDungThongBao;
extern int loaiThongBao;

// =============================================================
// 4. KHAI BÁO HÀM (GIỮ NGUYÊN)
// =============================================================

bool KhoiTaoGiaoDienSFML(sf::RenderWindow &window, sf::Font &font, const std::string &fontPath);
void ChayChuongTrinhSFML(sf::RenderWindow &window, sf::Font &font);
void CapNhatThongBaoSFML(const std::string &msg, int loai = 0);
MaUI LayElementTaiToaDo(int mouseX, int mouseY);

#endif // GIAODIEN_SFML_H