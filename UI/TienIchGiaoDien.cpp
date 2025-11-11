#include "include/TienIchGiaoDien.h"
#include <algorithm> // dung cho std::min de gioi han gia tri mau

// file nay chua cac ham dung chung de ve cac thanh phan ui co ban nhu
// van ban, khung, nut bam, o nhap lieu. muc dich la tai su dung code
// va giup cac file ve man hinh khac gon gang hon.

// dinh nghia cac ham tien ich

// tao mot doi tuong sf::text (van ban) voi cac thuoc tinh co ban
sf::Text TaoVanBan(const sf::Font& font, const std::string& chuoi, unsigned int coChu, sf::Color mauSac) {
    sf::Text text;
    text.setFont(font);         // dat font chu
    text.setString(chuoi);      // dat noi dung chuoi
    text.setCharacterSize(coChu); // dat co chu
    text.setFillColor(mauSac);  // dat mau chu
    return text;                // tra ve doi tuong van ban
}

// can chinh goc (origin) cua doi tuong sf::text vao giua hinh chu nhat bao quanh no
// muc dich: de khi setposition(x, y), thi diem (x, y) se la tam cua van ban
// huu ich cho viec can giua nut bam
void CanGiuaGocVanBan(sf::Text& text) {
    // lay kich thuoc va vi tri cua hinh chu nhat bao quanh van ban
    sf::FloatRect bounds = text.getLocalBounds();
    // dat goc toa do moi (origin) vao chinh giua hinh chu nhat do
    // bounds.left la do lech x cua chu (thuong la 0 hoac so nho)
    // bounds.top la do lech y cua chu (do dac tinh font)
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
}

// can chinh goc (origin) cua doi tuong sf::text vao giua theo chieu ngang va top theo chieu doc
// muc dich: de khi setposition(x, y), thi diem (x, y) se la tam-tren cung cua van ban
// huu ich cho viec can giua tieu de man hinh
void CanGiuaGocXVanBan(sf::Text& text) {
    sf::FloatRect bounds = text.getLocalBounds();
    // dat goc toa do x vao giua, goc y o tren cung (bounds.top)
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top);
}

// ve mot khung hinh chu nhat voi vien va tieu de (neu co)
void VeKhung(sf::RenderWindow& window, float x, float y, float w, float h, const std::string& tieuDe, const sf::Font& font) {

    // tao hinh chu nhat nen
    sf::RectangleShape khungNen(sf::Vector2f(w, h)); // kich thuoc w, h
    khungNen.setPosition(x, y);                   // dat vi tri goc tren trai
    khungNen.setFillColor(MAU_KHUNG);             // dat mau nen (dinh nghia trong GiaoDienSFML.h)
    khungNen.setOutlineThickness(1.f);            // dat do day vien la 1 pixel
    khungNen.setOutlineColor(MAU_VIEN);           // dat mau vien (dinh nghia trong GiaoDienSFML.h)
    window.draw(khungNen);                        // ve khung len cua so

    // neu co tieu de thi ve tieu de
    if (!tieuDe.empty()) {
        // tao doi tuong van ban tieu de
        sf::Text txtTieuDe = TaoVanBan(font, tieuDe, FONT_SIZE_TIEU_DE_KHUNG, MAU_TIEU_DE);
        // dat vi tri tieu de (lech vao trong theo padding)
        txtTieuDe.setPosition(x + PADDING, y + PADDING/2.f);
        window.draw(txtTieuDe);                   // ve tieu de len cua so
    }
}

// tao mot doi tuong nut bam (uielement) va them vao mang cacelement toan cuc
// ham nay khong ve nut, chi tao du lieu cho nut. viec ve se do vong lap trong GiaoDienSFML.cpp dam nhiem
void TaoNut(const sf::Font& font, MaUI idNut, float x, float y, float rong, float cao, const std::string& nhan, sf::Color mauNen, sf::Color mauChu) {
    // kiem tra xem mang cacelement con cho khong
    if (soLuongElement < SO_ELEMENT_TOI_DA) {
        // lay tham chieu den phan tu tiep theo trong mang (toi uu hon copy)
        UIElement& nut = cacElement[soLuongElement];
        nut.id = idNut;        // gan id cho nut
        nut.laInput = false;   // danh dau day la nut (khong phai input)

        // cau hinh hinh dang (hinh chu nhat) cua nut
        nut.hinhDang.setSize(sf::Vector2f(rong, cao)); // dat kich thuoc
        nut.hinhDang.setPosition(x, y);                // dat vi tri

        // logic highlight khi di chuot qua (hover)
        if (elementHover == idNut) { // bien elementhover duoc cap nhat trong GiaoDienSFML.cpp
            // lam mau nen sang hon mot chut
            int r = std::min(255, mauNen.r + 20);      // std::min de khong vuot qua 255
            int g = std::min(255, mauNen.g + 20);
            int b = std::min(255, mauNen.b + 20);
            nut.hinhDang.setFillColor(sf::Color(r, g, b)); // dat mau highlight
        } else {
            nut.hinhDang.setFillColor(mauNen);      // dat mau nen binh thuong
        }
        nut.hinhDang.setOutlineThickness(1.f);      // dat vien 1 pixel
        nut.hinhDang.setOutlineColor(MAU_VIEN);     // dat mau vien

        // tao va cau hinh van ban (nhan) tren nut
        nut.vanBan = TaoVanBan(font, nhan, FONT_SIZE_BINH_THUONG, mauChu);
        CanGiuaGocVanBan(nut.vanBan); // can chinh goc vao giua van ban
        // dat vi tri van ban vao giua nut
        nut.vanBan.setPosition(x + rong / 2.0f, y + cao / 2.0f);

        soLuongElement++;
    }
}

// tao mot doi tuong o nhap lieu (uielement) va them vao mang cacelement toan cuc
// bao gom nhan (label) ben trai va hop nhap lieu ben phai
// ham nay khong ve, chi tao du lieu
void TaoInput(const sf::Font& font, MaUI idInput, float x, float y, float rong, float cao, const std::string& label, const std::string& giaTri, const std::string& goiY, bool readOnly) {
    if (soLuongElement < SO_ELEMENT_TOI_DA) {
        UIElement& input = cacElement[soLuongElement];
        input.id = idInput;      // gan id
        input.laInput = true;    // danh dau day la input
        float labelWidth = 100.f; // chieu rong co dinh cho phan nhan (label) ben trai

        // tao va dat vi tri cho nhan (label)
        input.vanBan = TaoVanBan(font, label, FONT_SIZE_BINH_THUONG, MAU_CHU);
        // can chinh y cua nhan vao giua chieu cao 'cao'
        input.vanBan.setPosition(x, y + (cao - input.vanBan.getCharacterSize()) / 2.f);

        // tinh toa do x bat dau cua hop input (ben phai nhan)
        float inputX = x + labelWidth + 10.f; // 10.f la khoang cach giua nhan va hop

        // cau hinh hinh dang (hop input)
        input.hinhDang.setSize(sf::Vector2f(rong, cao)); // dat kich thuoc hop
        input.hinhDang.setPosition(inputX, y);           // dat vi tri hop
        // mau nen: mo neu readonly, binh thuong neu khong
        input.hinhDang.setFillColor(readOnly ? MAU_KHUNG : MAU_INPUT_NEN);
        input.hinhDang.setOutlineThickness(1.f);         // vien 1 pixel

        // logic chon mau vien cho hop input
        sf::Color borderColor = MAU_INPUT_BORDER; // mac dinh la mau vien thuong
        // kiem tra xem co loi va input nay dang active khong
        bool coLoiOInputNay = (loaiThongBao == 1 && inputHoatDong == idInput); // loaithongbao = 1 la loi

        if (inputHoatDong == idInput && !readOnly) { // neu input dang duoc chon (active) va khong readonly
            // neu co loi thi vien mau do, khong thi vien mau xanh active
            borderColor = coLoiOInputNay ? MAU_LOI : MAU_INPUT_ACTIVE_BORDER;
        } else if (readOnly) { // neu la readonly thi vien mau mo
            borderColor = MAU_VIEN;
        }
        input.hinhDang.setOutlineColor(borderColor); // dat mau vien da chon

        // logic hien thi chu ben trong hop input (gia tri hoac goi y)
        std::string displayText = giaTri; // mac dinh hien thi gia tri dang nhap
        sf::Color displayColor = readOnly ? MAU_VIEN : MAU_INPUT_CHU; // mau chu mac dinh

        // neu hop input rong & khong active & co chuoi goi y -> hien thi goi y
        if (giaTri.empty() && inputHoatDong != idInput && !goiY.empty()) {
            displayText = goiY;         // hien thi chuoi goi y
            displayColor = MAU_VIEN;    // hien thi mau mo (placeholder)
        }

        // them dau nhay "_" neu input dang active va khong readonly
        if (inputHoatDong == idInput && !readOnly) displayText += "_";

        // tao va dat vi tri cho van ban ben trong hop input
        input.vanBanPhu = TaoVanBan(font, displayText, FONT_SIZE_BINH_THUONG, displayColor);
        // can chinh y cua van ban vao giua chieu cao 'cao'
        input.vanBanPhu.setPosition(inputX + 5.f, y + (cao - input.vanBanPhu.getCharacterSize()) / 2.f); // 5.f la padding trai

        soLuongElement++;
    }
}

// Ham xu ly word wrap cho text (cat text thanh nhieu dong neu qua dai)
// Tra ve chuoi da format voi ky tu xuong dong '\n'
// KHONG DUNG VECTOR, chi dung string va ky tu '\n' de xuong dong
std::string WordWrapText(const sf::Font& font, const std::string& text, unsigned int fontSize, float maxWidth) {
    if (text.empty() || maxWidth <= 0) {
        return text;
    }

    // Tao text tam de do chieu rong
    sf::Text tempText;
    tempText.setFont(font);
    tempText.setCharacterSize(fontSize);
    
    std::string result = "";      // Ket qua cuoi cung
    std::string currentLine = ""; // Dong hien tai dang xu ly
    std::string currentWord = ""; // Tu hien tai dang xu ly
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        
        // Neu gap ky tu xuong dong, them dong hien tai vao ket qua
        if (c == '\n') {
            if (!currentWord.empty()) {
                if (!currentLine.empty()) currentLine += " ";
                currentLine += currentWord;
                currentWord = "";
            }
            if (!currentLine.empty()) {
                if (!result.empty()) result += "\n";
                result += currentLine;
                currentLine = "";
            } else {
                if (!result.empty()) result += "\n";
                result += " "; // Dong trong
            }
            continue;
        }
        
        // Neu gap khoang trang, kiem tra xem co can xuong dong khong
        if (c == ' ') {
            std::string testLine = currentLine;
            if (!testLine.empty() && !currentWord.empty()) testLine += " ";
            testLine += currentWord;
            tempText.setString(testLine);
            
            if (tempText.getLocalBounds().width > maxWidth && !currentLine.empty()) {
                // Dong hien tai da day, them vao ket qua va bat dau dong moi
                if (!result.empty()) result += "\n";
                result += currentLine;
                currentLine = currentWord;
                currentWord = "";
            } else {
                // Them tu vao dong hien tai
                if (!currentLine.empty()) currentLine += " ";
                currentLine += currentWord;
                currentWord = "";
            }
        } else {
            // Them ky tu vao tu hien tai
            currentWord += c;
        }
    }
    
    // Them tu cuoi cung
    if (!currentWord.empty()) {
        std::string testLine = currentLine;
        if (!testLine.empty()) testLine += " ";
        testLine += currentWord;
        tempText.setString(testLine);
        
        if (tempText.getLocalBounds().width > maxWidth && !currentLine.empty()) {
            // Dong hien tai da day
            if (!result.empty()) result += "\n";
            result += currentLine;
            currentLine = currentWord;
        } else {
            // Them vao dong hien tai
            if (!currentLine.empty()) currentLine += " ";
            currentLine += currentWord;
        }
    }
    
    // Them dong cuoi cung
    if (!currentLine.empty()) {
        if (!result.empty()) result += "\n";
        result += currentLine;
    }
    
    return result.empty() ? text : result;
}