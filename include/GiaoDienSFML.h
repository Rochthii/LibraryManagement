#ifndef GIAODIEN_SFML_H
#define GIAODIEN_SFML_H

#include <SFML/Graphics.hpp>
#include <string>
#include "QuanLySach.h" 


// file nay dinh nghia cac hang so ve kich thuoc, mau sac, font chu,
// cac enum de phan biet trang thai man hinh va id cua cac ui element,
// struct uielement dung chung, va khai bao cac bien/ham toan cuc
// lien quan den giao dien chinh.

// kich Thuoc & Mau Sac 
// dinh nghia kich thuoc cua so va cac mau sac co ban
const unsigned int CHIEU_RONG = 1600; // chieu rong cua so
const unsigned int CHIEU_CAO = 900;  // chieu cao cua so
// cac hang so mau sac (sf::color dinh nghia bang r, g, b)
const sf::Color MAU_NEN(30, 30, 45);           // mau nen chinh (xanh den / tim than dam)
const sf::Color MAU_TIEU_DE(255, 200, 100);    // mau chu tieu de (vang cam nhat)
const sf::Color MAU_NEN_NUT(100, 100, 110);    // mau nen mac dinh cua nut (xam dam, hoi xanh)
const sf::Color MAU_CHU_NUT(220, 220, 220);    // mau chu tren nut (xam rat nhat / trang xam)
const sf::Color MAU_CHU(200, 200, 200);        // mau chu thong thuong (xam nhat)
const sf::Color MAU_NHAN(100, 150, 255);       // mau dung de highlight (xanh duong nhat / xanh da troi)
const sf::Color MAU_NUT_BACK(80, 80, 90);      // mau rieng cho nut Back/Menu (xam dam)
const sf::Color MAU_KHUNG(50, 50, 65);         // mau nen cua cac khung (xam xanh den rat dam)
const sf::Color MAU_VIEN(150, 150, 160);       // mau vien cua khung, nut, input (xam trung binh, hoi xanh)
const sf::Color MAU_INPUT_NEN(60, 60, 75);     // mau nen cua o input (xam xanh den dam)
const sf::Color MAU_INPUT_CHU(210, 210, 210);  // mau chu trong o input (xam rat nhat / trang xam)
const sf::Color MAU_INPUT_BORDER(120, 120, 130); // mau vien mac dinh cua input (xam, hoi xanh)
const sf::Color MAU_INPUT_ACTIVE_BORDER(100, 150, 255); // mau vien input khi active (xanh duong nhat)
const sf::Color MAU_LOI(255, 100, 100);         // mau cho thong bao loi, nut Xoa (do nhat / hong do)
const sf::Color MAU_THANH_CONG(100, 255, 100);  // mau cho thong bao thanh cong (xanh la cay nhat)
const sf::Color MAU_BANG_HEADER(45, 45, 60);    // mau nen cua dong tieu de bang (xam xanh den rat dam)
const sf::Color MAU_BANG_BORDER(80, 80, 90);    // mau cua cac duong ke trong bang (xam dam)

// Hang so Co Chu
// dinh nghia cac co chu de dam bao dong nhat
const unsigned int FONT_SIZE_NHO = 16;           // co chu nho (vd: trong modal chi tiet)
const unsigned int FONT_SIZE_BINH_THUONG = 18;   // co chu mac dinh (nut, input, bang)
const unsigned int FONT_SIZE_TIEU_DE_KHUNG = 22; // co chu tieu de khung (vd: "them dau sach")
const unsigned int FONT_SIZE_TIEU_DE_LON = 32;   // co chu tieu de lon (vd: man hinh thong tin)
const unsigned int FONT_SIZE_TIEU_DE_MAIN = 42;  // co chu tieu de chinh (vd: man hinh menu)

// Hang so Layout cho Man Hinh Quan Ly Sach
// dinh nghia cac kich thuoc va vi tri co dinh cho man hinh quan ly sach
// giup viec tinh toan toa do ve de dang hon
const float THANH_TAB_CAO = 40.f;       // chieu cao thanh tieu de man hinh
const float PADDING = 20.f;             // khoang cach dem mac dinh
const float FORM_RONG = 450.f;          // chieu rong cua form ben phai
const float BANG_X = PADDING;           // toa do x bat dau cua bang ben trai
const float BANG_Y = THANH_TAB_CAO + PADDING; // toa do y bat dau cua bang ben trai
const float BANG_RONG = CHIEU_RONG - FORM_RONG - 3 * PADDING;    // chieu rong bang = rong cua so - rong form - 3 lan padding (trai, giua, phai)
const float FORM_X = BANG_X + BANG_RONG + PADDING;               // toa do x bat dau cua form ben phai
// toa do y bat dau cua form (bang voi y cua header bang)
const float FORM_Y = BANG_Y + 50.f; // Can chinh voi Y cua bang sach
const float KHUNG_THONG_BAO_Y = CHIEU_CAO - PADDING - 140.f;   // toa do y bat dau cua khung thong bao (duoi cung ben phai)
const float KHUNG_THONG_BAO_CAO = 140.f;      // chieu cao khung thong bao
const float NUT_CAO = 40.f;                   // chieu cao mac dinh cua nut
const float INPUT_CAO = 40.f;                 // chieu cao mac dinh cua input

const float INPUT_RONG = FORM_RONG - 2 * PADDING - 110.f;  // chieu rong hop input = rong form - 2 padding - rong label - khoang cach
const int SACH_MOI_TRANG = 17;                             // so luong sach hien thi tren moi trang cua bang chinh

//Trang Thai Man Hinh
// enum de quan ly man hinh nao dang duoc hien thi
enum TrangThaiManHinh {
    MENU_CHINH,         // man hinh menu ban dau
    MAN_HINH_THONG_TIN, // man hinh gioi thieu thong tin
    QUAN_LY_SACH        // man hinh quan ly dau sach
    //co the them MAN_HINH_DOC_GIA, MAN_HINH_MUON_TRA,...
};

// Ma Dinh Danh UI Elements
// enum de gan id duy nhat cho moi nut bam, o nhap lieu, hoac khu vuc tuong tac khac
// giup ham xu ly su kien biet duoc element nao vua duoc tuong tac
enum MaUI {
    // Nut Menu
    NUT_QUAN_LY_DAU_SACH,
    NUT_QUAN_LY_DOC_GIA,  // chua lam
    NUT_MUON_TRA_SACH,    // chua lam
    NUT_THONG_TIN,
    NUT_THOAT,
    NUT_BACK,             // nut quay lai menu / huy sua

    // Elements tren man hinh Quan Ly Sach
    INPUT_TIM_SACH,       // o nhap tu khoa tim kiem
    NUT_TIM,              // nut tim kiem
    NUT_XOA_TIM,          // nut xoa trang o tim kiem va reset bang
    NUT_CHUYEN_CHE_DO_XEM,
    INPUT_ISBN,
    INPUT_TEN_SACH,
    INPUT_SO_TRANG,
    INPUT_TAC_GIA,
    INPUT_NAM_XB,
    INPUT_THE_LOAI,
    INPUT_SO_LUONG,       // chi hien khi them moi
    NUT_THEM_SACH,        // cung la nut "luu thay doi" khi sua
    NUT_SUA,              // nut chuan bi sua sach da chon
    NUT_XOA,              // nut hien hop thoai xac nhan xoa
    NUT_XAC_NHAN_XOA,     // nut xac nhan xoa trong hop thoai
    NUT_HUY_XOA,          // nut huy xoa trong hop thoai
    NUT_TRANG_TRUOC,      // nut chuyen trang bang sach
    NUT_TRANG_SAU,
    NUT_HUY_CHON,         // nut bo chon sach dang highlight
    HANG_SACH,            // id ao dai dien cho viec click vao mot hang trong bang sach
    NUT_MODAL_TRANG_TRUOC, 
    NUT_MODAL_TRANG_SAU,

    // Nut cho Modal Chi Tiet Ban Sao
    NUT_CHI_TIET_SACH,        // nut mo modal chi tiet
    NUT_DONG_MODAL_CHITIET,   // nut dong modal chi tiet
    NUT_XAC_NHAN_THANHLY,     // nut xac nhan thanh ly trong hop thoai con
    NUT_HUY_THANHLY,          // nut huy thanh ly trong hop thoai con
    // Nut cho modal Them Ban Sao 
    NUT_THEM_BAN_SAO,         // Nut mo modal
    INPUT_SO_LUONG_THEM,      // O nhap so luong trong modal
    NUT_XAC_NHAN_THEM_BS,     // Nut xac nhan them trong modal
    NUT_HUY_THEM_BS,          // Nut huy them trong modal
    KHONG_XAC_DINH            // id mac dinh khi khong click vao element nao
};

// Cau truc UI Element chung
// struct de luu thong tin cua mot thanh phan giao dien (nut hoac input)
// mang cac uielement se duoc dung de ve va bat su kien click
struct UIElement {
    sf::RectangleShape hinhDang; // hinh chu nhat nen cua nut/input
    sf::Text vanBan;             // chu chinh (nhan nut, label input)
    sf::Text vanBanPhu;          // chu phu (noi dung input, placeholder)
    MaUI id;                     // id de phan biet
    bool laInput = false;        // danh dau la nut hay input
};

#define SO_ELEMENT_TOI_DA 50     // gioi han so luong element toi da co the tao trong mot man hinh

// Khai bao bien toan cuc (extern)
// cac bien nay se duoc dinh nghia trong file GiaoDienSFML.cpp
// chung luu trang thai chung cua giao dien
extern TrangThaiManHinh manHinhHienTai;         // man hinh nao dang hien thi
extern UIElement cacElement[SO_ELEMENT_TOI_DA]; // mang luu cac nut/input can ve
extern int soLuongElement;                      // so luong element hien co trong mang
extern bool yeuCauThoat;                        // co de thoat chuong trinh
extern MaUI inputHoatDong;                      // id cua input dang duoc chon (active)
extern MaUI elementHover;                       // id cua element dang duoc di chuot qua
extern std::string noiDungThongBao;             // noi dung hien thi trong khung thong bao
extern int loaiThongBao;                        // loai thong bao (0: info, 1: loi, 2: thanh cong) de chon mau


// Khai Bao Ham Giao Dien SFML (dinh nghia trong GiaoDienSFML.cpp)
bool KhoiTaoGiaoDienSFML(sf::RenderWindow &window, sf::Font &font, const std::string &fontPath); // ham khoi tao cua so va tai font
void ChayChuongTrinhSFML(sf::RenderWindow &window, sf::Font &font); // ham vong lap chinh cua chuong trinh sfml
void CapNhatThongBaoSFML(const std::string &msg, int loai = 0);     // ham cap nhat noi dung va loai thong bao (dung boi cac file khac)
MaUI LayElementTaiToaDo(int mouseX, int mouseY);                    // ham lay id cua element tai toa do chuot (dung de bat click/hover)

#endif // GIAODIEN_SFML_H