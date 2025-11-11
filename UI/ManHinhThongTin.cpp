#include "include/ManHinhThongTin.h"
#include "include/GiaoDienSFML.h"
#include "include/TienIchGiaoDien.h"

void VeManHinhThongTinSFML(sf::RenderWindow &window, const sf::Font &font){
    // tinh toa do giua man hinh
    float centerX = CHIEU_RONG / 2.0f;
    float currentY = 100.f; // Y bat dau

    // ve Tieu de truong
    sf::Text tieuDe = TaoVanBan(font, "Hoc vien Cong nghe Buu Chinh Vien Thong - TPHCM", FONT_SIZE_TIEU_DE_LON, MAU_TIEU_DE);
    CanGiuaGocXVanBan(tieuDe); // can giua theo chieu X
    tieuDe.setPosition(centerX, currentY);
    window.draw(tieuDe);
    currentY += 100.f; // xuong hang

    // dinh nghia toa do X cho nhan (label) va gia tri (value)
    float labelX = centerX - 400.f;
    float valueX = centerX - 150.f;
    unsigned int coChuInfo = 26; // co chu cho cac dong thong tin

    // ve thong tin Mon hoc
    sf::Text labelMonHoc = TaoVanBan(font, "* Mon hoc:", coChuInfo, MAU_CHU);
    labelMonHoc.setPosition(labelX, currentY);
    sf::Text valueMonHoc = TaoVanBan(font, "Cau Truc Du Lieu & Giai Thuat", coChuInfo, MAU_NHAN);
    valueMonHoc.setPosition(valueX, currentY);
    window.draw(labelMonHoc);
    window.draw(valueMonHoc);
    currentY += 50.f; // xuong hang

    // ve thong tin Giang vien
    sf::Text labelGV = TaoVanBan(font, "* Giang vien:", coChuInfo, MAU_CHU);
    labelGV.setPosition(labelX, currentY);
    sf::Text valueGV = TaoVanBan(font, "Luu Nguyen Ky Thu", coChuInfo, MAU_NHAN);
    valueGV.setPosition(valueX, currentY);
    window.draw(labelGV);
    window.draw(valueGV);
    currentY += 50.f; // Xuong hang

    // ve thong tin Do An
    sf::Text labelDA = TaoVanBan(font, "* Do An:", coChuInfo, MAU_CHU);
    labelDA.setPosition(labelX, currentY);
    sf::Text valueDA = TaoVanBan(font, "Quan Ly Thu Vien", coChuInfo, MAU_NHAN);
    valueDA.setPosition(valueX, currentY);
    window.draw(labelDA);
    window.draw(valueDA);
    currentY += 100.f; // xuong hang (khoang cach lon hon)

    // ve thong tin Sinh vien
    sf::Text labelSV = TaoVanBan(font, "* Sinh vien:", coChuInfo, MAU_CHU);
    labelSV.setPosition(labelX, currentY);
    sf::Text valueSV1 = TaoVanBan(font, "1. Cham Roch Thi - N24DECE041 - E24CQCE01-N", coChuInfo, MAU_NHAN);
    valueSV1.setPosition(valueX, currentY);
    currentY += 40.f; // xuong hang (khoang cach hep hon)
    sf::Text valueSV2 = TaoVanBan(font, "2. Tran Gia Binh - N24DECE05 - E24CQCE01-N", coChuInfo, MAU_NHAN);
    valueSV2.setPosition(valueX, currentY);
    window.draw(labelSV);
    window.draw(valueSV1);
    window.draw(valueSV2);
    currentY += 120.f; // xuong hang (khoang cach lon hon)

    sf::Text nam = TaoVanBan(font, "-- 2025 --", 20, MAU_CHU);   // ve Nam
    CanGiuaGocXVanBan(nam); // can giua
    nam.setPosition(centerX, currentY);
    window.draw(nam);
    TaoNut(font, NUT_BACK, 20.f, 20.f, 120.f, 40.f, "< MENU", MAU_NUT_BACK, MAU_CHU_NUT);  // ve nut Quay Lai Menu
}

//xu Ly Su Kien
void XuLySuKienThongTin(sf::RenderWindow &window, sf::Event event) {
    (void)window; // danh dau khong dung bien window

    if (event.type == sf::Event::MouseButtonPressed) {   // xu ly click chuot
        if (event.mouseButton.button == sf::Mouse::Left) {
            MaUI elementNhan = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);
            if (elementNhan == NUT_BACK) {   // neu click vao nut BACK -> Quay ve menu chinh
                manHinhHienTai = MENU_CHINH;
            }
        }
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {  // xu ly nhan phim ESCAPE -> Quay ve menu chinh
        manHinhHienTai = MENU_CHINH;
    }
}