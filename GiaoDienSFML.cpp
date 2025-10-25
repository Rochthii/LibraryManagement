#include "include/GiaoDienSFML.h"
#include <iostream>

// --- Biến Trạng Thái ---
static TrangThaiManHinh manHinhHienTai = MENU_CHINH;
static NutBam cacNutBamHienTai[SO_NUT_TOI_DA];
static int soLuongNutBamHienTai = 0;

// --- Hàm Hỗ Trợ Nội Bộ ---

sf::Text TaoVanBan(const sf::Font &font, const std::string &chuoi, unsigned int coChu, sf::Color mauSac)
{
    sf::Text text;
    text.setFont(font);
    text.setString(chuoi);
    text.setCharacterSize(coChu);
    text.setFillColor(mauSac);
    return text;
}

void CanGiuaGocXVanBan(sf::Text &text)
{
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
}

void TaoNut(const sf::Font &font, MaNut idNut, float x, float y, float rong, float cao, const std::string &nhan, sf::Color mauNen, sf::Color mauChu)
{
    if (soLuongNutBamHienTai < SO_NUT_TOI_DA)
    {
        NutBam &nut = cacNutBamHienTai[soLuongNutBamHienTai];
        nut.id = idNut;

        nut.hinhDang.setSize(sf::Vector2f(rong, cao));
        nut.hinhDang.setPosition(x, y);
        nut.hinhDang.setFillColor(mauNen);

        nut.vanBan = TaoVanBan(font, nhan, 20, mauChu);
        sf::FloatRect textBounds = nut.vanBan.getLocalBounds();
        nut.vanBan.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        nut.vanBan.setPosition(x + rong / 2.0f, y + cao / 2.0f);

        soLuongNutBamHienTai++;
    }
    else
    {
        std::cerr << "Loi: Mang nut bam da day!" << std::endl;
    }
}

void VeMenuChinhSFML(sf::RenderWindow &window, const sf::Font &font)
{
    soLuongNutBamHienTai = 0;

    sf::Text tieuDe = TaoVanBan(font, "QUAN LY THU VIEN", 40, MAU_TIEU_DE);
    CanGiuaGocXVanBan(tieuDe);
    tieuDe.setPosition(CHIEU_RONG / 2.0f, 130.f);
    window.draw(tieuDe);

    float nutRong = 350.f;
    float nutCao = 60.f;
    float khoangCachNut = 25.f;
    float x = (CHIEU_RONG - nutRong) / 2.0f;
    float y = 250.f;

    TaoNut(font, NUT_QUAN_LY_DAU_SACH, x, y, nutRong, nutCao, "1. QUAN LY DAU SACH", MAU_NEN_NUT, MAU_CHU_NUT);
    y += nutCao + khoangCachNut;
    TaoNut(font, NUT_QUAN_LY_DOC_GIA, x, y, nutRong, nutCao, "2. QUAN LY DOC GIA", MAU_NEN_NUT, MAU_CHU_NUT);
    y += nutCao + khoangCachNut;
    TaoNut(font, NUT_MUON_TRA_SACH, x, y, nutRong, nutCao, "3. MUON TRA SACH", MAU_NEN_NUT, MAU_CHU_NUT);
    y += nutCao + khoangCachNut;
    TaoNut(font, NUT_THONG_TIN, x, y, nutRong, nutCao, "4. THONG TIN", MAU_NEN_NUT, MAU_CHU_NUT);
    y += nutCao + khoangCachNut;
    TaoNut(font, NUT_THOAT, x, y, nutRong, nutCao, "5. THOAT", MAU_NEN_NUT, MAU_CHU_NUT);

    for (int i = 0; i < soLuongNutBamHienTai; ++i)
    {
        window.draw(cacNutBamHienTai[i].hinhDang);
        window.draw(cacNutBamHienTai[i].vanBan);
    }
}

void VeManHinhThongTinSFML(sf::RenderWindow &window, const sf::Font &font)
{
    soLuongNutBamHienTai = 0;

    float centerX = CHIEU_RONG / 2.0f;
    float currentY = 100.f;

    sf::Text tieuDe = TaoVanBan(font, "Hoc vien Cong nghe Buu Chinh Vien Thong - TPHCM", 30, MAU_TIEU_DE);
    CanGiuaGocXVanBan(tieuDe);
    tieuDe.setPosition(centerX, currentY);
    window.draw(tieuDe);
    currentY += 100.f;

    float labelX = centerX - 400.f;
    float valueX = centerX - 150.f;
    unsigned int coChuInfo = 24;

    sf::Text labelMonHoc = TaoVanBan(font, "* Mon hoc:", coChuInfo, MAU_CHU);
    labelMonHoc.setPosition(labelX, currentY);
    sf::Text valueMonHoc = TaoVanBan(font, "Cau Truc Du Lieu & Giai Thuat", coChuInfo, MAU_NHAN);
    valueMonHoc.setPosition(valueX, currentY);
    window.draw(labelMonHoc);
    window.draw(valueMonHoc);
    currentY += 50.f;

    sf::Text labelGV = TaoVanBan(font, "* Giang vien:", coChuInfo, MAU_CHU);
    labelGV.setPosition(labelX, currentY);
    sf::Text valueGV = TaoVanBan(font, "Luu Nguyen Ky Thu", coChuInfo, MAU_NHAN);
    valueGV.setPosition(valueX, currentY);
    window.draw(labelGV);
    window.draw(valueGV);
    currentY += 50.f;

    sf::Text labelDA = TaoVanBan(font, "* Do An:", coChuInfo, MAU_CHU);
    labelDA.setPosition(labelX, currentY);
    sf::Text valueDA = TaoVanBan(font, "Quan Ly Thu Vien", coChuInfo, MAU_NHAN);
    valueDA.setPosition(valueX, currentY);
    window.draw(labelDA);
    window.draw(valueDA);
    currentY += 100.f;

    sf::Text labelSV = TaoVanBan(font, "* Sinh vien:", coChuInfo, MAU_CHU);
    labelSV.setPosition(labelX, currentY);
    sf::Text valueSV1 = TaoVanBan(font, "1. Cham Roch Thi - N24DECE041 - E24CQCE01-N", coChuInfo, MAU_NHAN);
    valueSV1.setPosition(valueX, currentY);
    currentY += 40.f;
    sf::Text valueSV2 = TaoVanBan(font, "2. Tran Gia Binh - N24DECE05 - E24CQCE01-N", coChuInfo, MAU_NHAN);
    valueSV2.setPosition(valueX, currentY);
    window.draw(labelSV);
    window.draw(valueSV1);
    window.draw(valueSV2);
    currentY += 120.f;

    sf::Text nam = TaoVanBan(font, "-- 2025 --", 20, MAU_CHU);
    CanGiuaGocXVanBan(nam);
    nam.setPosition(centerX, currentY);
    window.draw(nam);

    TaoNut(font, NUT_BACK, 20.f, 20.f, 120.f, 40.f, "< MENU", MAU_NUT_BACK, MAU_CHU_NUT);
    window.draw(cacNutBamHienTai[0].hinhDang);
    window.draw(cacNutBamHienTai[0].vanBan);
}

MaNut LayNutDuocNhan(int mouseX, int mouseY)
{
    for (int i = 0; i < soLuongNutBamHienTai; ++i)
    {
        if (cacNutBamHienTai[i].hinhDang.getGlobalBounds().contains(static_cast<float>(mouseX), static_cast<float>(mouseY)))
        {
            return cacNutBamHienTai[i].id;
        }
    }
    return NUT_KHONG_XAC_DINH;
}

bool KhoiTaoGiaoDienSFML(sf::RenderWindow &window, sf::Font &font, const std::string &fontPath)
{
    (void)window;
    std::cout << "Dang khoi tao cua so SFML..." << std::endl;
    if (!font.loadFromFile(fontPath))
    {
        std::cerr << "Loi: Khong the tai file font: " << fontPath << std::endl;
        return false;
    }
    std::cout << "Khoi tao SFML thanh cong!" << std::endl;
    return true;
}

void ChayChuongTrinhSFML(sf::RenderWindow &window, sf::Font &font)
{
    std::cout << "[DEBUG] Bat dau vong lap SFML..." << std::endl;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    MaNut nutNhan = LayNutDuocNhan(event.mouseButton.x, event.mouseButton.y);
                    switch (nutNhan)
                    {
                    case NUT_QUAN_LY_DAU_SACH:
                        manHinhHienTai = QUAN_LY_SACH;
                        std::cout << "[DEBUG] Chuyen sang Quan Ly Sach (Chua lam)\n";
                        break;
                    case NUT_THONG_TIN:
                        manHinhHienTai = MAN_HINH_THONG_TIN;
                        break;
                    case NUT_THOAT:
                        window.close();
                        break;
                    case NUT_BACK:
                        if (manHinhHienTai != MENU_CHINH)
                        {
                            manHinhHienTai = MENU_CHINH;
                        }
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

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    if (manHinhHienTai == MENU_CHINH)
                    {
                        window.close();
                    }
                    else
                    {
                        manHinhHienTai = MENU_CHINH;
                    }
                }
                if (manHinhHienTai == MENU_CHINH)
                {
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1)
                        manHinhHienTai = QUAN_LY_SACH;
                    else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2)
                        std::cout << "[DEBUG] Chuc nang chua duoc thuc hien!\n";
                    else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3)
                        std::cout << "[DEBUG] Chuc nang chua duoc thuc hien!\n";
                    else if (event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Numpad4)
                        manHinhHienTai = MAN_HINH_THONG_TIN;
                    else if (event.key.code == sf::Keyboard::Num5 || event.key.code == sf::Keyboard::Numpad5)
                        window.close();
                }
            }
        } // Ket thuc pollEvent

        window.clear(MAU_NEN);

        switch (manHinhHienTai)
        {
        case MENU_CHINH:
            VeMenuChinhSFML(window, font);
            break;
        case MAN_HINH_THONG_TIN:
            VeManHinhThongTinSFML(window, font);
            break;
        case QUAN_LY_SACH:
        {
            sf::Text tempText = TaoVanBan(font, "Man hinh Quan Ly Sach (Chua lam - Nhan ESC de quay lai)", 20, MAU_CHU);
            tempText.setPosition(100.f, 100.f);
            window.draw(tempText);
        }
        break;
        }

        window.display();
    } // Ket thuc window.isOpen()

    std::cout << "[DEBUG] Ket thuc vong lap SFML." << std::endl;
}
