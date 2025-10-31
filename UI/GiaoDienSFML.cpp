#include "include/GiaoDienSFML.h"
#include "include/TienIchGiaoDien.h"
#include "include/ManHinhMenuChinh.h"
#include "include/ManHinhThongTin.h"
#include "include/ManHinhQuanLySach.h"
//cua binh
#include "include/GiaoDienQuanLyDocGia.h"
#include "include/GiaoDienMuonTra.h"
#include <iostream>

// dinh nghia cac bien toan cuc
TrangThaiManHinh manHinhHienTai = MENU_CHINH;
UIElement cacElement[SO_ELEMENT_TOI_DA];
int soLuongElement = 0;
bool yeuCauThoat = false;
MaUI inputHoatDong = KHONG_XAC_DINH;
MaUI elementHover = KHONG_XAC_DINH;
std::string noiDungThongBao = "Chao mung!";
int loaiThongBao = 0;

bool KhoiTaoGiaoDienSFML(sf::RenderWindow &window, sf::Font &font, const std::string &fontPath) {
    (void)window; // danh sau k dung
    std::cout << "Dang khoi tao cua so SFML..." << std::endl;
    if (!font.loadFromFile(fontPath)) {
        std::cerr << "Loi: Khong the tai file font: " << fontPath << std::endl;
        return false;
    }
    std::cout << "Khoi tao SFML thanh cong!" << std::endl;
    KhoiTaoManHinhSach();
    return true;
}

// ham ve tong quat
static void VeManHinhHienTai(sf::RenderWindow &window, const sf::Font &font) {
    window.clear(MAU_NEN);
    soLuongElement = 0; // reset cac eleyement moi frame

    switch (manHinhHienTai) {
    case MENU_CHINH:
        VeMenuChinhSFML(window, font);
        break;
    case MAN_HINH_THONG_TIN:
        VeManHinhThongTinSFML(window, font);
        break;
    case QUAN_LY_SACH:
        VeManHinhQuanLySach(window, font);
        break;
    }

    // ve tat ca cac UIElement da duoc tao
    for (int i = 0; i < soLuongElement; ++i) {
        window.draw(cacElement[i].hinhDang);
        window.draw(cacElement[i].vanBan);
        if (cacElement[i].laInput) {
            window.draw(cacElement[i].vanBanPhu);
        }
    }
}

// ham xu ly su kien tong quat
static void XuLySuKienSFML(sf::RenderWindow &window) {
    sf::Event event;

    // Cap nhat hover
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    MaUI idHover = LayElementTaiToaDo(mousePos.x, mousePos.y);
    bool isHoveredElementInput = false;
    for (int i = 0; i < soLuongElement; ++i) {
        if (cacElement[i].id == idHover && cacElement[i].laInput) {
            isHoveredElementInput = true;
            break;
        }
    }
    if (!isHoveredElementInput)
        elementHover = idHover;
    else
        elementHover = KHONG_XAC_DINH;

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            yeuCauThoat = true;
        }

        // Dieu phoi su kien den dung man hinh
        switch (manHinhHienTai) {
        case MENU_CHINH:
            XuLySuKienMenuChinh(window, event);
            break;
        case MAN_HINH_THONG_TIN:
            XuLySuKienThongTin(window, event);
            break;
        case QUAN_LY_SACH:
            XuLySuKienManHinhSach(window, event);
            break;
        }

        // xu ly Escape toan cuc (neu man hinh con khong xu ly)
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (manHinhHienTai == MENU_CHINH) {
                window.close();
                yeuCauThoat = true;
            }
            // Neu khong phai menu, cac ham con (vd: XuLySuKienManHinhSach) se tu bat Escape
            // de quay ve menu
        }
    }
}

void ChayChuongTrinhSFML(sf::RenderWindow &window, sf::Font &font) {
    std::cout << "[DEBUG] Bat dau vong lap SFML..." << std::endl;
    yeuCauThoat = false;

    while (window.isOpen() && !yeuCauThoat) {
        XuLySuKienSFML(window);
        VeManHinhHienTai(window, font);
        window.display();
    }

    std::cout << "[DEBUG] Ket thuc vong lap SFML." << std::endl;
}

void CapNhatThongBaoSFML(const std::string &msg, int loai) {
    noiDungThongBao = msg;
    loaiThongBao = loai;
}

MaUI LayElementTaiToaDo(int mouseX, int mouseY) {
    // Kiem tra cac element nut/input truoc
    for (int i = 0; i < soLuongElement; ++i) {
        if (cacElement[i].hinhDang.getGlobalBounds().contains(static_cast<float>(mouseX), static_cast<float>(mouseY))) {
            return cacElement[i].id;
        }
    }

    // Kiem tra logic rieng
    if (manHinhHienTai == QUAN_LY_SACH) {
        // Tinh toan Y_dau va Y_cuoi cua bang
        float tableTop = BANG_Y + 90 + 30;                                // Y bat dau cua hang so 1
        float tableBottom = CHIEU_CAO - (PADDING * 5.5f) - (NUT_CAO * 2); // Y ket thuc cua bang

        if (mouseX >= BANG_X + PADDING && mouseX < BANG_X + BANG_RONG - PADDING && mouseY >= tableTop && mouseY < tableBottom) {
            return HANG_SACH;
        }
    }
    return KHONG_XAC_DINH;
}
