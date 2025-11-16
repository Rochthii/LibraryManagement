#include "include/ManHinhQuanLySach.h"
#include "include/GiaoDienSFML.h"
#include "include/TienIchGiaoDien.h"
#include "include/TrangThaiManHinhSach.h"
#include "include/QuanLySach.h"
#include "include/KiemTraDuLieu.h"
#include "include/XuLyChuoi.h"
#include "include/Constants.h"
#include "include/NgayThang.h"
#include <iostream>
#include <algorithm>
#include <sstream>


// Bien trang thai duy nhat cho toan bo man hinh
static SachState state;

// khai bao ham noi bo (static)
static void VeBangSach(sf::RenderWindow &window, const sf::Font &font, const SachState& currentState);
static void VeFormThemSach(sf::RenderWindow &window, const sf::Font &font, const SachState& currentState);
static void VeKhungThongBaoSFML(sf::RenderWindow &window, const sf::Font &font, const SachState& currentState);
static void VeModalChiTietBanSao(sf::RenderWindow &window, const sf::Font &font, SachState& currentState);
static void VeModalThemBanSao(sf::RenderWindow &window, const sf::Font &font, const SachState& currentState);
static void VeDanhSachTheoTheLoai(sf::RenderWindow& window, const sf::Font& font, SachState& currentState);
static void CapNhatDuLieuXemTheoTheLoai(SachState& currentState);

static void XuLyTextInput(sf::Event event, SachState& currentState);
static void ThucHienTimKiemNoiBo(SachState& currentState);
static void ThucHienThemHoacSuaSachSFML(SachState& currentState);
static void ThucHienXoaSachSFML(SachState& currentState);
static void ThucHienThanhLySach(SachState& currentState, std::string maSach);
static void ThucHienXoaBanSao(SachState& currentState, std::string maSach);
static void ThucHienThemBanSao(SachState& currentState);

static void XoaFormNhapLieuSFML(SachState& currentState);
static void DienFormVoiSachDuocChon(SachState& currentState);
static void CapNhatPhanTrangSFML(SachState& currentState);

//Ham tien ich cat chuoi + "..." de tai su dung
static inline std::string CatChuoiVoiDauCham(const std::string& str, size_t maxLen) {
    return (str.length() > maxLen) ? str.substr(0, maxLen - 3) + "..." : str;
}

// Ham cat chuoi theo pixel (dung cho VeDanhSachTheoTheLoai)
static std::string CatChuoiTheoRong(const sf::Font& font, const std::string& src, unsigned int coChu, float rongToiDa) {
    sf::Text tmp = TaoVanBan(font, src, coChu, MAU_CHU);
    if (tmp.getLocalBounds().width <= rongToiDa)
        return src; // tra ve neu da vua
    std::string s = src;
    if (s.size() > 3) s = s.substr(0, s.size() - 3); // rut bot de them '...'
    tmp.setString(s + "...");
    while (tmp.getLocalBounds().width > rongToiDa && s.size() > 0) { // cat dan
        s.pop_back();
        tmp.setString(s + "...");
    }
    return s + (s.empty() ? std::string() : std::string("..."));
}

static void KhoiTaoStack(UndoStack *s) {
    s->top = -1;
}
static bool KiemTraStackRong(UndoStack *s) {
    return s->top == -1;
}
static bool KiemTraStackDay(UndoStack *s) {
    return s->top == MAX_UNDO_STEPS - 1;
}

// Ham them mot trang thai vao dinh stack
static void PushStack(UndoStack *s, const std::string& trangThai) {
    if (!KiemTraStackDay(s)) { // Chi them neu stack chua day
        // Kiem tra tranh push trang thai giong het trang thai tren dinh
        if (KiemTraStackRong(s) || s->history[s->top].data != trangThai) {
            s->top++;
            s->history[s->top].data = trangThai;
        }
    }
    // Neu stack day, bo qua trang thai cu hon (khong lam gi)
}

// Ham lay va xoa trang thai khoi dinh stack
static std::string PopStack(UndoStack *s) {
    if (!KiemTraStackRong(s)) {
        std::string data = s->history[s->top].data;
        s->top--;    // Giam dinh stack
        return data; // Tra ve trang thai da lay
    }
    return ""; // Tra ve chuoi rong neu stack rong
}

// DINH NGHIA HAM

// Ham Ve (View) - Chi doc state de ve, khong sua doi

static void VeBangSach(sf::RenderWindow &window, const sf::Font &font, const SachState& currentState){
    float headerY = BANG_Y + 50.f;
    float contentY = headerY + 40.f;
    // Tinh day bang, tru khong gian cho nut
    float tableBottom = CHIEU_CAO - (PADDING * 5.5f) - (NUT_CAO * 2);

    VeKhung(window, BANG_X, headerY, BANG_RONG, tableBottom - headerY, "DANH SACH DAU SACH (Double-click de chon)", font);

    // Phan chia ty le cot (tong = 1.0)
    float colWidths[] = {0.05f, 0.16f, 0.30f, 0.07f, 0.18f, 0.07f, 0.11f, 0.06f};
    float colX[9];
    colX[0] = BANG_X + PADDING;
    for (int i = 0; i < 8; ++i) {
        colX[i + 1] = colX[i] + colWidths[i] * (BANG_RONG - PADDING);
        if (i < 7) {
            sf::RectangleShape line(sf::Vector2f(1.f, tableBottom - contentY));
            line.setPosition(colX[i + 1] - PADDING / 2.f, contentY);
            line.setFillColor(MAU_BANG_BORDER);
            window.draw(line);
        }
    }

    sf::Text headerText;
    headerText.setFont(font);
    headerText.setCharacterSize(FONT_SIZE_BINH_THUONG);
    headerText.setFillColor(MAU_NHAN);

    std::string headers[] = {"STT", "ISBN", "Ten Sach", "Trang", "Tac Gia", "NXB", "The Loai", "SL"};

    float headerBoxHeight = 30.f;
    sf::RectangleShape headerBg(sf::Vector2f(BANG_RONG - PADDING * 0.8, headerBoxHeight));
    headerBg.setPosition(BANG_X + PADDING * 0.4, contentY);
    headerBg.setFillColor(MAU_BANG_HEADER);
    window.draw(headerBg);

    for (int i = 0; i < 8; ++i) {
        headerText.setString(headers[i]);
        headerText.setPosition(colX[i], contentY + (headerBoxHeight - headerText.getCharacterSize()) / 2.f);
        window.draw(headerText);
    }
    sf::RectangleShape headerLine(sf::Vector2f(BANG_RONG - 2 * PADDING, 1.f));
    headerLine.setPosition(BANG_X + PADDING, contentY + 30.f);
    headerLine.setFillColor(MAU_BANG_BORDER);
    window.draw(headerLine);

    float currentY = contentY + 35.f;
    int startIndex = (currentState.trangHienTai - 1) * SACH_MOI_TRANG;
    int endIndex = (startIndex + SACH_MOI_TRANG < currentState.soLuongKetQuaTimKiem)
                       ? (startIndex + SACH_MOI_TRANG)
                       : currentState.soLuongKetQuaTimKiem;

    float rowHeight = 30.f;

    sf::Text dataText;
    dataText.setFont(font);
    dataText.setCharacterSize(FONT_SIZE_BINH_THUONG);

    for (int i = startIndex; i < endIndex; ++i) {
        PTRDS d = currentState.ketQuaTimKiem[i].sach;
        if (!d)
            continue;

        bool isSelected = (d->ISBN == currentState.isbnSachDuocChon);
        if (isSelected) {
            sf::RectangleShape highlight(sf::Vector2f(BANG_RONG - PADDING * 0.8, rowHeight));
            highlight.setPosition(BANG_X + PADDING * 0.4, currentY - 2.f);
            highlight.setFillColor(MAU_NHAN);
            window.draw(highlight);
        }
        dataText.setFillColor(isSelected ? sf::Color::Black : MAU_CHU);

        // Dung ham tien ich de cat chuoi
        std::string tenSachSub = CatChuoiVoiDauCham(d->tenSach, 25);
        std::string tacGiaSub = CatChuoiVoiDauCham(d->tacGia, 15);
        std::string theLoaiSub = CatChuoiVoiDauCham(d->theLoai, 8);

        float textY = currentY + (rowHeight - dataText.getCharacterSize()) / 2.f;

        dataText.setString(std::to_string(i + 1));
        dataText.setPosition(colX[0], textY);
        window.draw(dataText);
        dataText.setString(d->ISBN);
        dataText.setPosition(colX[1], textY);
        window.draw(dataText);
        dataText.setString(tenSachSub);
        dataText.setPosition(colX[2], textY);
        window.draw(dataText);
        dataText.setString(std::to_string(d->soTrang));
        dataText.setPosition(colX[3], textY);
        window.draw(dataText);
        dataText.setString(tacGiaSub);
        dataText.setPosition(colX[4], textY);
        window.draw(dataText);
        dataText.setString(std::to_string(d->namXuatBan));
        dataText.setPosition(colX[5], textY);
        window.draw(dataText);
        dataText.setString(theLoaiSub);
        dataText.setPosition(colX[6], textY);
        window.draw(dataText);
        dataText.setString(std::to_string(d->tongBanSao));
        dataText.setPosition(colX[7], textY);
        window.draw(dataText);

        currentY += rowHeight;
        if (currentY > tableBottom - PADDING)
            break;
    }

    float buttonY = tableBottom + PADDING;
    float pageY = buttonY + NUT_CAO + PADDING / 2;

    if (!currentState.isbnSachDuocChon.empty()) {
        float btnWidth = (BANG_RONG - (PADDING * 4)) / 5.f; // Chia deu 5 nut
        float btnSpacing = PADDING;
        float btnX = BANG_X;

        TaoNut(font, NUT_THEM_BAN_SAO, btnX, buttonY, btnWidth, NUT_CAO, "THEM BAN SAO", MAU_NHAN, MAU_CHU_NUT);
        btnX += btnWidth + btnSpacing;
        TaoNut(font, NUT_CHI_TIET_SACH, btnX, buttonY, btnWidth, NUT_CAO, "CHI TIET", MAU_NEN_NUT, MAU_CHU_NUT); // Doi mau
        btnX += btnWidth + btnSpacing;
        TaoNut(font, NUT_SUA, btnX, buttonY, btnWidth, NUT_CAO, "SUA", MAU_NEN_NUT, MAU_CHU_NUT);
        btnX += btnWidth + btnSpacing;
        TaoNut(font, NUT_XOA, btnX, buttonY, btnWidth, NUT_CAO, "XOA", MAU_LOI, MAU_CHU_NUT);
        btnX += btnWidth + btnSpacing;
        TaoNut(font, NUT_HUY_CHON, btnX, buttonY, btnWidth, NUT_CAO, "HUY CHON", MAU_NEN_NUT, MAU_CHU_NUT);
    }

    TaoNut(font, NUT_TRANG_TRUOC, BANG_X, pageY, 120.f, NUT_CAO, "<< Trang Truoc", MAU_NEN_NUT, MAU_CHU_NUT);
    TaoNut(font, NUT_TRANG_SAU, BANG_X + 130.f, pageY, 120.f, NUT_CAO, "Trang Sau >>", MAU_NEN_NUT, MAU_CHU_NUT);

    sf::Text pageInfo = TaoVanBan(font, "Trang " + std::to_string(currentState.trangHienTai) + " / " + std::to_string(currentState.tongSoTrang), 16, MAU_CHU);
    pageInfo.setPosition(BANG_X + 260.f, pageY + NUT_CAO / 2.f - pageInfo.getLocalBounds().height / 2.f - 2.f);
    window.draw(pageInfo);
}

static void VeFormThemSach(sf::RenderWindow &window, const sf::Font &font, const SachState& currentState) {
    // Lay constants
    static int namHienTai = LayNamHienTai();
    std::string goiYNamXB = std::to_string(NAM_XUAT_BAN_MIN) + "-" + std::to_string(namHienTai);
    std::string goiYSoTrang = "1 - " + std::to_string(MAX_SO_TRANG);
    std::string goiYSoLuong = "1 - " + std::to_string(MAX_BAN_SAO) + " ban sao";
    std::string goiYISBN = "10 hoac 13 chu so";
    
    // Hint text dong cho cac truong co text
    std::string goiYTenSach = currentState.chuoiTenSach.empty() 
        ? "Toi da " + std::to_string(MAX_TEN_SACH) + " ky tu, bat dau bang chu"
        : std::to_string(currentState.chuoiTenSach.length()) + "/" + std::to_string(MAX_TEN_SACH);
    std::string goiYTacGia = currentState.chuoiTacGia.empty()
        ? "Toi da " + std::to_string(MAX_TAC_GIA) + " ky tu, bat dau bang chu"
        : std::to_string(currentState.chuoiTacGia.length()) + "/" + std::to_string(MAX_TAC_GIA);
    std::string goiYTheLoai = currentState.chuoiTheLoai.empty()
        ? "Toi da " + std::to_string(MAX_THE_LOAI) + " ky tu, bat dau bang chu"
        : std::to_string(currentState.chuoiTheLoai.length()) + "/" + std::to_string(MAX_THE_LOAI);

    std::string formTieuDe = currentState.dangSua ? "HIEU CHINH DAU SACH" : "THEM DAU SACH";

    // Tinh chieu cao form cho khop
    float formHeight = KHUNG_THONG_BAO_Y - FORM_Y - PADDING;
    VeKhung(window, FORM_X, FORM_Y, FORM_RONG, formHeight, formTieuDe, font);
    float currentY = FORM_Y + 40.f;
    float labelX = FORM_X + PADDING;
    float inputSpacing = PADDING / 2.f;

    TaoInput(font, INPUT_ISBN, labelX, currentY, INPUT_RONG, INPUT_CAO, "ISBN (*):", currentState.chuoiISBN, goiYISBN, currentState.dangSua);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_TEN_SACH, labelX, currentY, INPUT_RONG, INPUT_CAO, "Ten Sach (*):", currentState.chuoiTenSach, goiYTenSach);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_SO_TRANG, labelX, currentY, INPUT_RONG, INPUT_CAO, "So Trang (*):", currentState.chuoiSoTrang, goiYSoTrang);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_TAC_GIA, labelX, currentY, INPUT_RONG, INPUT_CAO, "Tac Gia (*):", currentState.chuoiTacGia, goiYTacGia);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_NAM_XB, labelX, currentY, INPUT_RONG, INPUT_CAO, "Nam XB (*):", currentState.chuoiNamXB, goiYNamXB);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_THE_LOAI, labelX, currentY, INPUT_RONG, INPUT_CAO, "The Loai (*):", currentState.chuoiTheLoai, goiYTheLoai);
    currentY += INPUT_CAO + inputSpacing;

    if (!currentState.dangSua) {
        TaoInput(font, INPUT_SO_LUONG, labelX, currentY, INPUT_RONG, INPUT_CAO, "So Luong (*):", currentState.chuoiSoLuong, goiYSoLuong);
        currentY += INPUT_CAO + PADDING * 1.5f;
    }
    else {
        currentY += PADDING * 1.5f;
    }

    float totalWidth = FORM_RONG - 2 * PADDING;
    float nutRong = (totalWidth - PADDING) / 2;
    float nutCao = NUT_CAO + 10.f;

    std::string nutChinhLabel = currentState.dangSua ? "LUU THAY DOI" : "THEM SACH";

    if (currentState.dangSua) {
        TaoNut(font, NUT_THEM_SACH, labelX, currentY, nutRong, nutCao, nutChinhLabel, MAU_NHAN, MAU_CHU_NUT);
        TaoNut(font, NUT_BACK, labelX + nutRong + PADDING, currentY, nutRong, nutCao, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
    }
    else {
        float nutRongDon = 250.f;
        float nutXDon = labelX + (totalWidth - nutRongDon) / 2;
        TaoNut(font, NUT_THEM_SACH, nutXDon, currentY, nutRongDon, nutCao, nutChinhLabel, MAU_NHAN, MAU_CHU_NUT);
    }
}

static void VeKhungThongBaoSFML(sf::RenderWindow &window, const sf::Font &font, const SachState& currentState) {
    VeKhung(window, FORM_X, KHUNG_THONG_BAO_Y, FORM_RONG, KHUNG_THONG_BAO_CAO, "THONG BAO", font);

    if (currentState.xacNhanXoa) { // Ve hop thoai xac nhan xoa
        sf::Text txtConfirm = TaoVanBan(font, "Ban co chac muon xoa Dau Sach nay?\n(Chi xoa duoc neu khong con ban sao)", FONT_SIZE_BINH_THUONG, MAU_LOI);
        // Can chinh vi tri Y chu
        txtConfirm.setPosition(FORM_X + PADDING, KHUNG_THONG_BAO_Y + 35.f);
        window.draw(txtConfirm);

        // Can chinh vi tri Y nut
        float buttonY = KHUNG_THONG_BAO_Y + 85.f;

        // Tinh lai chieu rong nut cho vua
        float totalWidth = FORM_RONG - 2 * PADDING;
        float nutRong = (totalWidth - PADDING) / 2;

        TaoNut(font, NUT_XAC_NHAN_XOA, FORM_X + PADDING, buttonY, nutRong, NUT_CAO, "XAC NHAN XOA", MAU_LOI, MAU_CHU_NUT);
        TaoNut(font, NUT_HUY_XOA, FORM_X + PADDING + nutRong + PADDING, buttonY, nutRong, NUT_CAO, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
    }
    else {
        // Ve thong bao binh thuong (co word wrap)
        if (noiDungThongBao.empty()) {
            return; // Khong ve gi neu khong co noi dung
        }

        sf::Color mauChuThongBao = MAU_CHU;
        if (loaiThongBao == 1)
            mauChuThongBao = MAU_LOI;
        else if (loaiThongBao == 2)
            mauChuThongBao = MAU_THANH_CONG;

        // Tinh chieu rong toi da cho text (tru padding)
        float maxTextWidth = FORM_RONG - 2 * PADDING - 10.f; // Tru them 10px de an toan
        
        // Xu ly word wrap cho text
        std::string wrappedText = WordWrapText(font, noiDungThongBao, FONT_SIZE_BINH_THUONG, maxTextWidth);
        
        // Ve text (SFML tu dong xu ly '\n' de xuong dong)
        sf::Text txtMsg = TaoVanBan(font, wrappedText, FONT_SIZE_BINH_THUONG, mauChuThongBao);
        
        // Tinh vi tri Y de can giua theo chieu doc
        // Lay chieu cao thuc te cua text (bao gom nhieu dong)
        float textHeight = txtMsg.getLocalBounds().height;
        float startY = KHUNG_THONG_BAO_Y + 35.f; // Bat dau tu duoi tieu de
        float availableHeight = KHUNG_THONG_BAO_CAO - 35.f - PADDING; // Chieu cao co the dung
        
        // Gioi han so dong toi da (tranh tran ra ngoai)
        float maxLines = 4.f; // Toi da 4 dong
        float lineHeight = static_cast<float>(FONT_SIZE_BINH_THUONG) * 1.3f; // Chieu cao 1 dong (co khoang cach)
        float maxTextHeight = maxLines * lineHeight;
        
        // Neu text qua cao, cat bot (chi lay phan dau)
        if (textHeight > maxTextHeight) {
            // Dem so dong hien tai
            int soDong = 1;
            for (size_t i = 0; i < wrappedText.length(); ++i) {
                if (wrappedText[i] == '\n') soDong++;
            }
            
            // Neu nhieu hon maxLines, cat bot
            if (soDong > static_cast<int>(maxLines)) {
                int dongDaCat = 0;
                std::string textDaCat = "";
                for (size_t i = 0; i < wrappedText.length(); ++i) {
                    if (wrappedText[i] == '\n') {
                        dongDaCat++;
                        if (dongDaCat >= static_cast<int>(maxLines)) {
                            textDaCat += "..."; // Them "..." de bao hieu bi cat
                            break;
                        }
                    }
                    textDaCat += wrappedText[i];
                }
                wrappedText = textDaCat;
                txtMsg.setString(wrappedText);
                textHeight = txtMsg.getLocalBounds().height;
            }
        }
        
        // Can giua text trong khung thong bao (neu text nho hon khung)
        float textY = startY;
        if (textHeight < availableHeight) {
            textY = startY + (availableHeight - textHeight) / 2.f;
        } else {
            // Neu text cao hon khung, dat o vi tri dau
            textY = startY;
        }
        
        txtMsg.setPosition(FORM_X + PADDING, textY);
        window.draw(txtMsg);
    }
}

// Ham logic goi backend de them ban sao
static void ThucHienThemBanSao(SachState& currentState) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    extern bool duLieuDaThayDoi;

    int soLuongThem = 0;
    std::string chuoiSach = CatKhoangTrang(currentState.soLuongBanSaoCanThemStr);
    
    // Kiem tra chuoi so luong nhap vao - UX tot hon
    if (chuoiSach.empty()) {
        CapNhatThongBaoSFML("Loi: Chua nhap so luong ban sao!\nVui long nhap so luong ban sao can them (1-" + std::to_string(MAX_BAN_SAO) + ").\nVi du: 1, 5, 10.", 1);
        inputHoatDong = INPUT_SO_LUONG_THEM;
        return;
    }
    
    if (!ChuyenChuoiThanhSoNguyen(chuoiSach, soLuongThem, true)) {
        CapNhatThongBaoSFML("Loi: So luong khong hop le!\nGia tri '" + chuoiSach + "' khong phai la so nguyen hop le.\nVui long chi nhap so (1-" + std::to_string(MAX_BAN_SAO) + ").\nVi du: 1, 5, 10.", 1);
        inputHoatDong = INPUT_SO_LUONG_THEM;
        return;
    }
    
    if (soLuongThem <= 0) {
        CapNhatThongBaoSFML("Loi: So luong phai lon hon 0!\nGia tri '" + std::to_string(soLuongThem) + "' khong hop le.\nVui long nhap so nguyen duong (1-" + std::to_string(MAX_BAN_SAO) + ").\nVi du: 1, 5, 10.", 1);
        inputHoatDong = INPUT_SO_LUONG_THEM;
        return;
    }
    
    if (soLuongThem > MAX_BAN_SAO) {
        CapNhatThongBaoSFML("Loi: So luong vuot qua gioi han!\nGia tri '" + std::to_string(soLuongThem) + "' lon hon gioi han " + std::to_string(MAX_BAN_SAO) + ".\nVui long nhap so nho hon hoac bang " + std::to_string(MAX_BAN_SAO) + ".\nVi du: 1, 5, 10.", 1);
        inputHoatDong = INPUT_SO_LUONG_THEM;
        return;
    }

    // Tim Dau Sach can them
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, currentState.isbnSachDuocChon);
    if (!dauSach) {
        CapNhatThongBaoSFML("Loi: Khong tim thay dau sach ISBN '" + currentState.isbnSachDuocChon + "' de them ban sao!", 1);
        currentState.hienThiModalThemBS = false; // Dong modal
        return;
    }

    // Kiem tra tong so ban sao khong vuot qua MAX
    std::string loiTong = KiemTraTongSoBanSao(currentState.isbnSachDuocChon, soLuongThem);
    if (!loiTong.empty()) {
        CapNhatThongBaoSFML(loiTong, 1);
        inputHoatDong = INPUT_SO_LUONG_THEM;
        return;
    }

    // Tu dong lay vi tri tu ban sao cu hoac mac dinh (KHONG cho nguoi dung nhap)
    std::string viTriDeThem = "";
    std::string thongBaoViTri = "";
    if (dauSach->dms != nullptr) {
        viTriDeThem = dauSach->dms->viTri;
        thongBaoViTri = " (vi tri: '" + viTriDeThem + "' - tu dong lay tu ban sao cu)";
    } else {
        viTriDeThem = "Ke Chinh";
        thongBaoViTri = " (vi tri: 'Ke Chinh' - mac dinh)";
    }

    // DUYET HET de tim so lon nhat THUC SU (tranh trung khi du lieu khong sap xep)
    // Vi du: neu xoa ban sao giua hoac file doc khong theo thu tu
    int soThuTu = 1; // Mac dinh neu chua co ban sao nao
    if (dauSach->dms) { // Neu co ban sao
        PTRDMS pTemp = dauSach->dms;
        int dem = 0;
        int soLonNhat = 0;  // Tim so lon nhat trong tat ca cac ban sao
        
        while (pTemp && dem < MAX_BAN_SAO) { // Bao ve vong lap
            int soHienTai = LaySoHauToMaSach(pTemp);
            if (soHienTai > soLonNhat) {
                soLonNhat = soHienTai;  // Cap nhat so lon nhat
            }
            pTemp = pTemp->next;
            dem++;
        }
        
        if (soLonNhat > 0) {
            soThuTu = soLonNhat + 1;  // So tiep theo = so lon nhat + 1
        }
    }

    // Kiem tra overflow: soThuTu + soLuongThem khong vuot qua INT_MAX
    if (soThuTu > INT_MAX - soLuongThem) {
        CapNhatThongBaoSFML("Loi: So thu tu qua lon, khong the them them ban sao!\nHien tai: " + std::to_string(soThuTu) + ", them " + std::to_string(soLuongThem) + " se vuot qua gioi han.", 1);
        currentState.hienThiModalThemBS = false;
        return;
    }

    //vong lap them ban sao
    int themThanhCong = 0;
    std::string loiTrongLoop = "";
    
    for (int i = 0; i < soLuongThem; ++i) {
        // Tao ma sach moi
        std::string maSach = currentState.isbnSachDuocChon + "-" + std::to_string(soThuTu);

        // Dam bao KHONG TRUNG: neu ma da ton tai -> tang soThuTu den khi unique
        {
            std::stringstream dummy;
            int baoVe = 0;
            while (timDanhMucTheoMaSach(maSach, dsDauSach, soLuongDauSach, dummy, true)) {
                if (++baoVe > MAX_BAN_SAO) { // bao ve vong lap
                    loiTrongLoop = "Loi: Khong the sinh ma sach duy nhat!";
                    break;
                }
                if (soThuTu >= INT_MAX) {
                    loiTrongLoop = "Loi: So thu tu vuot gioi han!";
                    break;
                }
                ++soThuTu;
                maSach = currentState.isbnSachDuocChon + "-" + std::to_string(soThuTu);
            }
            if (!loiTrongLoop.empty()) break;
        }
        
        // Kiem tra do dai ma sach
        if (maSach.length() > static_cast<size_t>(DO_DAI_MA_SACH_MAX)) {
            loiTrongLoop = "Loi: Ma sach '" + maSach + "' qua dai (" + std::to_string(maSach.length()) + "/" + std::to_string(DO_DAI_MA_SACH_MAX) + " ky tu)!";
            break;
        }
        
        // Goi backend them ban sao vao DSLK (chen vao dau)
        std::string loiThemDMS = themDanhMucSach(dauSach->dms, maSach, CHO_MUON_DUOC, viTriDeThem);
        if (!loiThemDMS.empty()) { // Kiem tra loi
            loiTrongLoop = loiThemDMS;
            break; // Thoat neu loi
        }

        themThanhCong++;
        soThuTu++; // Tang so cho ban sao tiep theo
    }

    //  Chi cap nhat tong ban sao cho DAU SACH NAY (thay vi cap nhat TAT CA)
    dauSach->tongBanSao += themThanhCong;  // Cong them so luong da them thanh cong
    duLieuDaThayDoi = true; // Danh dau de luu file

    // Bao loi len UI neu co loi trong vong lap
    if (!loiTrongLoop.empty()) {
        std::string thongBao = "Chi them duoc " + std::to_string(themThanhCong) + "/" + std::to_string(soLuongThem) + " ban sao!";
        CapNhatThongBaoSFML(thongBao, 1); // Loai 1 = Mau do
    } else { // Neu khong co loi
        // Bao thanh cong len UI (gon nhe, tranh bi tran)
        std::string thongBao = "Da them " + std::to_string(themThanhCong) + " ban sao thanh cong!";
        CapNhatThongBaoSFML(thongBao, 2); // Loai 2 = Mau xanh
        currentState.vuaThucHienThanhCong = true; // Dat co de khong bi de thong bao Tim Kiem
    }

    currentState.hienThiModalThemBS = false;     // Dong modal
    currentState.soLuongBanSaoCanThemStr = "";   // Xoa input so luong
    inputHoatDong = KHONG_XAC_DINH; // Deactive input
    ThucHienTimKiemNoiBo(currentState);         // Tai lai bang chinh de cap nhat cot SL
}

// Ham ve modal Them Ban Sao
static void VeModalThemBanSao(sf::RenderWindow &window, const sf::Font &font, const SachState& currentState) {
    // Ve lop nen mo
    sf::RectangleShape overlay(sf::Vector2f(CHIEU_RONG, CHIEU_CAO));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // Dinh nghia kich thuoc va vi tri modal (giam cao vi chi co 1 input)
    float modalRong = 500.f;
    float modalCao = 200.f;  // Giam cao vi chi con input so luong
    // Can giua CHINH XAC voi khung bang ben trai (them offset nho de can bang)
    float modalX = BANG_X + (BANG_RONG - modalRong) / 2.f + 20.f; 
    float modalY = (CHIEU_CAO - modalCao) / 2.f;
    float paddingNoiBo = PADDING;

    // Ve khung modal
    VeKhung(window, modalX, modalY, modalRong, modalCao, "THEM BAN SAO", font);

    // Ve o Input so luong
    float inputY = modalY + 60.f;
    float inputHopRong = modalRong - 2 * paddingNoiBo;
    std::string hintSoLuong = currentState.soLuongBanSaoCanThemStr.empty() 
        ? "Nhap so luong (1-" + std::to_string(MAX_BAN_SAO) + ", chi nhan so)"
        : "";
    TaoInput(font, INPUT_SO_LUONG_THEM, modalX + paddingNoiBo, inputY, inputHopRong, INPUT_CAO, "", currentState.soLuongBanSaoCanThemStr, hintSoLuong);
    
    // Them thong bao nho ben duoi input
    inputY += INPUT_CAO + PADDING / 2.f;
    sf::Text txtNote = TaoVanBan(font, "Vi tri se tu dong gan theo ban sao cu hoac mac dinh 'Ke Chinh'", FONT_SIZE_NHO, sf::Color(150, 150, 150));
    txtNote.setPosition(modalX + paddingNoiBo, inputY);
    window.draw(txtNote);

    // Ve nut Xac Nhan / Huy
    float buttonY = modalY + modalCao - NUT_CAO - paddingNoiBo;
    float totalWidth = modalRong - 2 * paddingNoiBo;
    float nutRong = (totalWidth - paddingNoiBo) / 2.f;
    TaoNut(font, NUT_XAC_NHAN_THEM_BS, modalX + paddingNoiBo, buttonY, nutRong, NUT_CAO, "XAC NHAN", MAU_NHAN, MAU_CHU_NUT);
    TaoNut(font, NUT_HUY_THEM_BS, modalX + paddingNoiBo + nutRong + paddingNoiBo, buttonY, nutRong, NUT_CAO, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
}

// Cap nhat du lieu cho che do xem theo the loai (goi 1 lan khi du lieu thay doi)
static void CapNhatDuLieuXemTheoTheLoai(SachState& currentState) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    
    // Tinh chieu cao vung noi dung
    float headerY = BANG_Y + 50.f;
    float contentY = headerY + 40.f;
    float contentBottom = CHIEU_CAO - (PADDING * 3.0f) - (NUT_CAO * 2);
    float contentHeight = contentBottom - contentY;
    contentHeight = std::max(contentHeight, 10.f);
    
    // Lay danh sach cac the loai duy nhat va sap xep
    std::string cacTheLoai[MAX_DAUSACH];
    int soTheLoai = TimTheLoaiDuyNhat(dsDauSach, soLuongDauSach, cacTheLoai);
    SapXepTheLoaiTheoTen(cacTheLoai, soTheLoai);
    
    // Tinh toan tong chieu cao noi dung
    currentState.totalContentHeightTheLoai = PADDING;
    float lineSpacing = 8.f;
    float groupSpacing = 20.f;
    float afterTitleSpacing = 35.f;
    float afterGroupSpacing = 10.f;
    float textBlockHeight = FONT_SIZE_BINH_THUONG * 2 + lineSpacing;
    
    for (int i = 0; i < soTheLoai; ++i) {
        if (i > 0) currentState.totalContentHeightTheLoai += groupSpacing;
        currentState.totalContentHeightTheLoai += afterTitleSpacing;
        PTRDS tempSach[MAX_DAUSACH];
        int soSach = TimSachTheoTheLoai(dsDauSach, soLuongDauSach, cacTheLoai[i], tempSach);
        currentState.totalContentHeightTheLoai += (float)soSach * textBlockHeight;
        currentState.totalContentHeightTheLoai += afterGroupSpacing;
    }
    currentState.totalContentHeightTheLoai += PADDING;
    currentState.totalContentHeightTheLoai = std::max(currentState.totalContentHeightTheLoai, contentHeight);
}

static void VeDanhSachTheoTheLoai(sf::RenderWindow& window, const sf::Font& font, SachState& currentState) {
    // buoc 0: dinh nghia vung ve noi dung
    float headerY = BANG_Y + 50.f;
    float contentY = headerY + 40.f;
    float contentBottom = CHIEU_CAO - (PADDING * 3.0f) - (NUT_CAO * 2);
    float contentWidth = BANG_RONG;
    float contentHeight = contentBottom - contentY;
    contentHeight = std::max(contentHeight, 10.f);

    // buoc 1: ve khung chua danh sach
    VeKhung(window, BANG_X, headerY, contentWidth, contentHeight + (contentY - headerY), "DANH SACH DAU SACH THEO THE LOAI (Cuon chuot de xem)", font);

    // lay du lieu tu backend
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;

    // buoc 2: lay danh sach cac the loai duy nhat va sap xep (CHI DOC, KHONG TINH TOAN)
    std::string cacTheLoai[MAX_DAUSACH];
    int soTheLoai = TimTheLoaiDuyNhat(dsDauSach, soLuongDauSach, cacTheLoai);
    SapXepTheLoaiTheoTen(cacTheLoai, soTheLoai);

    // buoc 3: gioi han scrollOffsetYTheLoai (su dung totalContentHeightTheLoai da tinh)
    float maxScroll = 0.f;
    if (currentState.totalContentHeightTheLoai > contentHeight) {
        maxScroll = currentState.totalContentHeightTheLoai - contentHeight;
    }
    if (currentState.scrollOffsetYTheLoai < 0.f) currentState.scrollOffsetYTheLoai = 0.f;
    if (currentState.scrollOffsetYTheLoai > maxScroll) currentState.scrollOffsetYTheLoai = maxScroll;

    // buoc 4: Thiet lap SFML View (camera 2d cho vung cuon)
    // he toa do viewport cua sfml tinh tu TREN XUONG (0.0 -> 1.0)
    float viewportTop = contentY / (float)CHIEU_CAO; // y bat dau (tinh tu tren xuong, 0.0 -> 1.0)
    viewportTop = std::max(0.f, std::min(1.f, viewportTop));
    float viewportHeight = contentHeight / (float)CHIEU_CAO; // chieu cao (0.0 -> 1.0)
    viewportHeight = std::max(0.f, std::min(1.f, viewportHeight));

    float rongThanhCuon = 8.f; // chieu rong thanh cuon
    // chieu rong vung noi dung = chieu rong khung - chieu rong thanh cuon - padding nho
    float rongNoiDungView = contentWidth - rongThanhCuon - PADDING * 0.6f;
    rongNoiDungView = std::max(0.f, rongNoiDungView); // dam bao khong am

    // thiet lap "ong kinh" camera
    currentState.theLoaiView.setViewport(sf::FloatRect(
        BANG_X / (float)CHIEU_RONG,        // left (giong X cua khung)
        viewportTop,                       // top (da tinh o tren)
        rongNoiDungView / (float)CHIEU_RONG, // width (da tru cho thanh cuon)
        viewportHeight                     // height (da tinh o tren)
    ));
    
    // thiet lap "the gioi" ben trong camera
    currentState.theLoaiView.setSize(rongNoiDungView, contentHeight); // kich thuoc "the gioi"
    // dich chuyen camera theo vi tri cuon
    currentState.theLoaiView.setCenter(rongNoiDungView / 2.f, contentHeight / 2.f + currentState.scrollOffsetYTheLoai);

    // buoc 5: Ap dung View (bat camera)
    window.setView(currentState.theLoaiView);

    // buoc 6: Ve noi dung ben trong View (toa do tuong doi voi View)
    // view se tu dong cat moi thu ve ra ngoai khu vuc cua no
    float currentY = PADDING; // y bat dau tuong doi (so voi view)
    float contentX = PADDING; // x bat dau tuong doi (so voi view)
    float bookIndentX = contentX + 35.f; // thut le cho sach
    float lineWidth = rongNoiDungView - 2 * PADDING; // chieu rong duong ke (theo view)
    
    // Khai bao cac hang so layout (can thiet cho vong lap ve)
    float lineSpacing = 8.f;
    float groupSpacing = 20.f;
    float afterTitleSpacing = 35.f;
    float afterGroupSpacing = 10.f;
    float textBlockHeight = FONT_SIZE_BINH_THUONG * 2 + lineSpacing;

    for (int i = 0; i < soTheLoai; ++i) { // lap qua tung the loai
        std::string currentTheLoai = cacTheLoai[i];

        if (i > 0) { // ve duong ke phan cach
             sf::RectangleShape separator(sf::Vector2f(lineWidth, 1.f));
             separator.setPosition(contentX, currentY - 10.f);
             separator.setFillColor(MAU_BANG_BORDER);
             window.draw(separator);
             currentY += groupSpacing;
        }

        // ve tieu de the loai
        sf::Text genreTitle = TaoVanBan(font, "THE LOAI: " + currentTheLoai, FONT_SIZE_TIEU_DE_KHUNG, MAU_NHAN);
        genreTitle.setPosition(contentX, currentY);
        window.draw(genreTitle);
        currentY += afterTitleSpacing;

        // lay danh sach sach cua the loai nay
        PTRDS sachCungTheLoai[MAX_DAUSACH];
        int soSach = TimSachTheoTheLoai(dsDauSach, soLuongDauSach, currentTheLoai, sachCungTheLoai);

        // ve thong tin tung cuon sach
        sf::Text bookInfo = TaoVanBan(font, "", FONT_SIZE_BINH_THUONG, MAU_CHU);
        for (int j = 0; j < soSach; ++j) { // lap qua tung cuon sach
            PTRDS sach = sachCungTheLoai[j];
            if (!sach) continue;

            std::string sttStr = std::to_string(j + 1) + ". ";
            // tinh chieu rong con lai cho chu
            float rongChoNoiDung = rongNoiDungView - bookIndentX - PADDING;
            if (rongChoNoiDung < 20.f) rongChoNoiDung = 20.f;

            // cat chuoi cho vua (goi ham static)
            std::string tenSachSub = CatChuoiTheoRong(font, sach->tenSach, FONT_SIZE_BINH_THUONG, rongChoNoiDung);
            std::string metaInfo = "   ISBN: " + sach->ISBN + " | TG: " + sach->tacGia + " | NXB: " + std::to_string(sach->namXuatBan);
            std::string metaInfoSub = CatChuoiTheoRong(font, metaInfo, FONT_SIZE_BINH_THUONG, rongChoNoiDung);
            std::string infoLine1 = tenSachSub;
            std::string infoLine2 = metaInfoSub;
            float line1Y = currentY;

            // ve stt
            bookInfo.setString(sttStr);
            bookInfo.setPosition(contentX + 5.f, line1Y);
            window.draw(bookInfo);
            // ve dong 1 (ten sach)
            bookInfo.setString(infoLine1);
            bookInfo.setPosition(bookIndentX, line1Y);
            window.draw(bookInfo);
            // tinh y dong 2
            float line2Y = line1Y + FONT_SIZE_BINH_THUONG + (lineSpacing / 2);
            // ve dong 2 (thong tin phu)
            bookInfo.setString(infoLine2);
            bookInfo.setPosition(bookIndentX, line2Y);
            window.draw(bookInfo);
            // cap nhat y cho sach tiep theo
            currentY = line1Y + textBlockHeight;
        }
        currentY += afterGroupSpacing; // them khoang cach cuoi nhom
    }

    // buoc 7: Reset View (tat camera)
    window.setView(window.getDefaultView()); // tra ve camera mac dinh de ve thanh cuon

    // buoc 8: Ve thanh cuon (ben ngoai View, dung toa do cua so)
    if (currentState.totalContentHeightTheLoai > contentHeight) {
        float scrollBarHeight = contentHeight; // chieu cao thanh cuon = chieu cao vung noi dung
        // vi tri X cua thanh cuon (ben phai khung noi dung)
        float scrollBarX = BANG_X + contentWidth - rongThanhCuon - PADDING * 0.4f;
        float scrollBarY = contentY; // bat dau tu y cua vung content (thay vi headerY)

        // ve duong ray
        sf::RectangleShape scrollTrack(sf::Vector2f(rongThanhCuon, scrollBarHeight));
        scrollTrack.setPosition(scrollBarX, scrollBarY);
        scrollTrack.setFillColor(MAU_BANG_HEADER);
        window.draw(scrollTrack);

        // tinh toan tay cam
        float handleHeight = scrollBarHeight * (contentHeight / currentState.totalContentHeightTheLoai);
        handleHeight = std::max(handleHeight, 20.f); // chieu cao toi thieu
        float handleY = scrollBarY;
        if (maxScroll > 0) { // tinh vi tri y cua tay cam
             handleY = scrollBarY + (currentState.scrollOffsetYTheLoai / maxScroll) * (scrollBarHeight - handleHeight);
             handleY = std::min(handleY, scrollBarY + scrollBarHeight - handleHeight);
        }
        // ve tay cam
        sf::RectangleShape scrollHandle(sf::Vector2f(rongThanhCuon, handleHeight));
        scrollHandle.setPosition(scrollBarX, handleY);
        scrollHandle.setFillColor(MAU_VIEN);
        window.draw(scrollHandle);
    }
}


// Ham Ve CHINH
void VeManHinhQuanLySach(sf::RenderWindow &window, const sf::Font &font) {
    // ve thanh tieu de man hinh
    sf::RectangleShape topBar(sf::Vector2f(CHIEU_RONG, THANH_TAB_CAO));
    topBar.setFillColor(MAU_KHUNG);
    window.draw(topBar);
    sf::Text title = TaoVanBan(font, "QUAN LY DAU SACH", FONT_SIZE_BINH_THUONG, MAU_TIEU_DE);
    title.setPosition(PADDING, PADDING / 2.f - title.getLocalBounds().height / 2.f);
    window.draw(title);

    // ve nut quay ve menu
    TaoNut(font, NUT_BACK, CHIEU_RONG - PADDING - 100.f, PADDING / 4.f, 100.f, NUT_CAO * 0.8f, "< MENU", MAU_NUT_BACK, MAU_CHU_NUT);
    // ve o tim kiem va cac nut lien quan
    std::string goiYTimKiem = "Nhap ten sach, tac gia, ISBN hoac the loai de tim kiem...";
    TaoInput(font, INPUT_TIM_SACH, BANG_X, BANG_Y, 500.f, INPUT_CAO, "Tim Kiem:", state.chuoiTimKiem, goiYTimKiem);
    TaoNut(font, NUT_TIM, BANG_X + 110.f + 500.f + 10.f, BANG_Y, 100.f, NUT_CAO, "TIM", MAU_NHAN, MAU_CHU_NUT);
    TaoNut(font, NUT_XOA_TIM, BANG_X + 110.f + 500.f + 10.f + 100.f + 10.f, BANG_Y, 100.f, NUT_CAO, "XOA TIM", MAU_NEN_NUT, MAU_CHU_NUT);

    float nutChuyenX = BANG_X + 110.f + 500.f + 10.f + 100.f + 10.f + 100.f + PADDING; // sau nut Xoa Tim
    // thay doi nhan nut tuy theo che do hien tai
    std::string nhanNutChuyen = (state.cheDoXemHienTai == XEM_TIM_KIEM) ? "Xem theo The Loai" : "Xem Tim Kiem/Tat Ca";
    TaoNut(font, NUT_CHUYEN_CHE_DO_XEM, nutChuyenX, BANG_Y, 220.f /* rong hon chut */, NUT_CAO, nhanNutChuyen, MAU_NEN_NUT, MAU_CHU_NUT);

    // Goi ham ve bang/danh sach tuy theo che do ---
    if (state.cheDoXemHienTai == XEM_TIM_KIEM) {
        VeBangSach(window, font, state); // ve bang phan trang nhu cu neu la che do tim kiem
    }
    else {
        VeDanhSachTheoTheLoai(window, font, state); // ve danh sach gom nhom neu la che do the loai
    }

    VeFormThemSach(window, font, state);      // ve form them/sua sach ben phai
    VeKhungThongBaoSFML(window, font, state); // ve khung thong bao ben duoi form

    if (state.hienThiModalBanSao) { // modal chi tiet ban sao
        VeModalChiTietBanSao(window, font, state);
    }
    else if (state.hienThiModalThemBS) { // modal them ban sao
        VeModalThemBanSao(window, font, state);
    }
}

// Ham xu ly backend cho modal
static void ThucHienThanhLySach(SachState& /*currentState*/, std::string maSach) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    extern bool duLieuDaThayDoi;

    // Tim dau sach chua ban sao nay
    std::string isbn = TachISBNTuMaSach(maSach);
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbn);
    
    if (dauSach) {
        // Truyen dau linked list (dauSach->dms), khong phai 1 node
        bool thanhCong = capNhatTrangThaiSach(dauSach->dms, maSach, THANH_LY);
        
        if (thanhCong) {
            duLieuDaThayDoi = true;
            CapNhatThongBaoSFML("Da thanh ly ma sach: " + maSach, 2);
        } else {
            CapNhatThongBaoSFML("Loi: Khong tim thay ban sao de thanh ly!", 1);
        }
    } else {
        CapNhatThongBaoSFML("Loi: Khong tim thay dau sach!", 1);
    }
}

// Ham xu ly XOA BAN SAO
static void ThucHienXoaBanSao(SachState& /*currentState*/, std::string maSach) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    extern bool duLieuDaThayDoi;

    PTRDMS banSao = timDanhMucTheoMaSach(maSach, dsDauSach, soLuongDauSach, std::cout, true);
    
    if (banSao) {
        if (banSao->trangThai != DANG_MUON) {
            // Goi ham backend de xoa ban sao
            bool thanhCong = XoaSachTheoMaSach(dsDauSach, soLuongDauSach, maSach, std::cout);
            if (thanhCong) {
                // Cap nhat lai tong so ban sao
                CapNhatTongBanSao(dsDauSach, soLuongDauSach);
                duLieuDaThayDoi = true;
                CapNhatThongBaoSFML("Da xoa ban sao: " + maSach, 2);
                // Modal se tu reload khi ve lai (khong can dong)
            } else {
                CapNhatThongBaoSFML("Loi: Khong the xoa ban sao!", 1);
            }
        } else {
            CapNhatThongBaoSFML("Loi: Sach dang muon, khong the xoa!", 1);
        }
    } else {
        CapNhatThongBaoSFML("Loi: Khong tim thay ma sach de xoa!", 1);
    }
}


// Ham ve modal
static void VeModalChiTietBanSao(sf::RenderWindow &window, const sf::Font &font, SachState& currentState) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;

    // Ve lop nen mo phu kin man hinh
    sf::RectangleShape overlay(sf::Vector2f(CHIEU_RONG, CHIEU_CAO));
    overlay.setFillColor(sf::Color(0, 0, 0, 150)); // Mau den mo
    window.draw(overlay);

    // Dinh nghia kich thuoc va vi tri modal
    float modalRong = 800.f;
    float modalCao = 500.f;
    float modalX = (CHIEU_RONG - modalRong) / 2.f - 200.f; // Dich sang trai
    float modalY = (CHIEU_CAO - modalCao) / 2.f;
    float paddingNoiBo = 20.f;

    // Tim dau sach de lay ten va danh sach ban sao
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, currentState.idModalSachDuocChon);
    if (!dauSach) {
        CapNhatThongBaoSFML("Loi: Khong tim thay dau sach de xem chi tiet!", 1);
        currentState.hienThiModalBanSao = false; // Dong modal neu loi
        return;
    }

    // Ve khung modal va nut Dong
    // cat ngan ten sach cho tieu de modal
    std::string tieuDeModal = CatChuoiVoiDauCham(dauSach->tenSach, 45);
    VeKhung(window, modalX, modalY, modalRong, modalCao, "CHI TIET BAN SAO: " + tieuDeModal, font);
    
    // Them ghi chu huong dan cho user (de hieu workflow)
    sf::Text hintText = TaoVanBan(font, "Luu y: 'Thanh Ly' truoc, sau do 'Xoa' de loai bo khoi he thong", FONT_SIZE_NHO - 2, sf::Color(150, 150, 150));
    hintText.setPosition(modalX + paddingNoiBo, modalY + 35.f);
    window.draw(hintText);
    
    // Tan dung field tongBanSao thay vi duyet DSLK
    int tongBanSao = dauSach->tongBanSao;
    
    sf::Text infoText = TaoVanBan(font, "Tong: " + std::to_string(tongBanSao) + " ban sao", FONT_SIZE_NHO, MAU_NHAN);
    infoText.setPosition(modalX + modalRong - 200.f, modalY + 35.f);
    window.draw(infoText);

    // Dinh nghia toa do cac cot trong modal (chi can 1 cot hanh dong)
    float currentY = modalY + 60.f;
    float col1X = modalX + paddingNoiBo; // STT
    float col2X = col1X + 50.f;          // Ma Sach
    float col3X = col2X + 250.f;         // Trang Thai
    float col4X = col3X + 150.f;         // Vi Tri
    float col5X = col4X + 140.f;         // Hanh Dong (1 nut thoi)

    // Ve tieu de cot
    sf::Text header = TaoVanBan(font, "STT", FONT_SIZE_BINH_THUONG, MAU_NHAN);
    header.setPosition(col1X, currentY);
    window.draw(header);
    header.setString("Ma Sach");
    header.setPosition(col2X, currentY);
    window.draw(header);
    header.setString("Trang Thai");
    header.setPosition(col3X, currentY);
    window.draw(header);
    header.setString("Vi Tri");
    header.setPosition(col4X, currentY);
    window.draw(header);
    header.setString("Hanh Dong");
    header.setPosition(col5X, currentY);
    window.draw(header);
    currentY += 30.f; // Khoang cach sau header

    PTRDMS tempBanSao[MAX_BAN_SAO]; // Mang con tro tam
    int soLuongBSTrongDSLK = 0;
    PTRDMS p = dauSach->dms;
    // Doc con tro tu DSLK vao mang
    while (p != nullptr && soLuongBSTrongDSLK < MAX_BAN_SAO) {
        tempBanSao[soLuongBSTrongDSLK++] = p;
        p = p->next;
    }
    
    // Sap xep mang theo ma sach (thu tu tang dan)
    SapXepBanSaoTheoMa(tempBanSao, soLuongBSTrongDSLK);

    // Ve noi dung cac hang (duyet MANG TAM thay vi DSLK)
    sf::Text dataText = TaoVanBan(font, "", FONT_SIZE_NHO, MAU_CHU);
    int sttBatDau = (currentState.trangModal - 1) * SO_SACH_MOI_TRANG_MODAL;
    int sttKetThuc = sttBatDau + SO_SACH_MOI_TRANG_MODAL;
    sttKetThuc = std::min(sttKetThuc, soLuongBSTrongDSLK); // Khong vuot qua so luong thuc te

    currentState.soLuongNutModal = 0; // Reset mang nut dong

    for (int stt = sttBatDau; stt < sttKetThuc; ++stt) {
        p = tempBanSao[stt]; // Lay con tro tu mang tam
        // Ve STT (stt + 1 vi stt bat dau tu 0)
        dataText.setString(std::to_string(stt + 1));
        dataText.setPosition(col1X, currentY);
        window.draw(dataText);
        // Ve Ma Sach
        dataText.setString(p->maSach);
        dataText.setPosition(col2X, currentY);
        window.draw(dataText);

        // Ve Trang Thai (voi mau tuong ung)
        const char *trangThaiText = TenTrangThai(p->trangThai);
        sf::Color trangThaiColor = MAU_CHU;
        if (p->trangThai == DANG_MUON)
            trangThaiColor = MAU_LOI;           // Do - Canh bao
        else if (p->trangThai == THANH_LY)
            trangThaiColor = MAU_VIEN;          // Vang - Cho xoa
        else if (p->trangThai == CHO_MUON_DUOC)
            trangThaiColor = MAU_THANH_CONG;    // Xanh - San sang

        dataText.setString(trangThaiText);
        dataText.setFillColor(trangThaiColor);
        dataText.setPosition(col3X, currentY);
        window.draw(dataText);
        dataText.setFillColor(MAU_CHU); // Tra lai mau mac dinh

        // Ve Vi Tri
        dataText.setString(p->viTri);
        dataText.setPosition(col4X, currentY);
        window.draw(dataText);

        // Ve cac nut hanh dong tuy theo trang thai (LOGIC ROI RAC)
        float nutY = currentY - 2.f;
        float nutRong = 100.f;
        float nutCao = 25.f;
        
        if (p->trangThai == CHO_MUON_DUOC) {
            // CHO_MUON_DUOC -> Chi hien nut "Thanh Ly"
            sf::RectangleShape nutTL(sf::Vector2f(nutRong, nutCao));
            nutTL.setPosition(col5X, nutY);
            nutTL.setFillColor(MAU_VIEN);
            nutTL.setOutlineColor(MAU_VIEN);
            nutTL.setOutlineThickness(1.f);
            window.draw(nutTL);
            
            sf::Text txtTL = TaoVanBan(font, "[!] Thanh Ly", FONT_SIZE_NHO - 2, MAU_CHU_NUT);
            CanGiuaGocVanBan(txtTL);
            txtTL.setPosition(col5X + nutRong / 2.f, nutY + nutCao / 2.f);
            window.draw(txtTL);
            
            // Luu nut (chi co Thanh Ly)
            if (currentState.soLuongNutModal < SO_SACH_MOI_TRANG_MODAL) {
                currentState.cacNutTrongModal[currentState.soLuongNutModal].maSach = p->maSach;
                currentState.cacNutTrongModal[currentState.soLuongNutModal].khuVucNhanThanhLy = nutTL.getGlobalBounds();
                currentState.cacNutTrongModal[currentState.soLuongNutModal].khuVucNhanXoa = sf::FloatRect(); // Rong
                currentState.soLuongNutModal++;
            }
        }
        else if (p->trangThai == THANH_LY) {
            // THANH_LY -> Chi hien nut "Xoa"
            sf::RectangleShape nutXoa(sf::Vector2f(nutRong, nutCao));
            nutXoa.setPosition(col5X, nutY);
            nutXoa.setFillColor(MAU_LOI);
            nutXoa.setOutlineColor(MAU_LOI);
            nutXoa.setOutlineThickness(1.f);
            window.draw(nutXoa);
            
            sf::Text txtXoa = TaoVanBan(font, "[X] Xoa", FONT_SIZE_NHO - 2, MAU_CHU_NUT);
            CanGiuaGocVanBan(txtXoa);
            txtXoa.setPosition(col5X + nutRong / 2.f, nutY + nutCao / 2.f);
            window.draw(txtXoa);
            
            // Luu nut (chi co Xoa)
            if (currentState.soLuongNutModal < SO_SACH_MOI_TRANG_MODAL) {
                currentState.cacNutTrongModal[currentState.soLuongNutModal].maSach = p->maSach;
                currentState.cacNutTrongModal[currentState.soLuongNutModal].khuVucNhanThanhLy = sf::FloatRect(); // Rong
                currentState.cacNutTrongModal[currentState.soLuongNutModal].khuVucNhanXoa = nutXoa.getGlobalBounds();
                currentState.soLuongNutModal++;
            }
        }
        // DANG_MUON -> Khong co nut nao (hien dau '-')
        
        currentY += 30.f; // Xuong hang
    } // Ket thuc vong for

    // Ve Hop Thoai Xac Nhan Thanh Ly
    if (currentState.xacNhanThanhLy) {
        sf::RectangleShape confirmOverlay(sf::Vector2f(modalRong, modalCao));
        confirmOverlay.setPosition(modalX, modalY);
        confirmOverlay.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(confirmOverlay);
        float confirmRong = 400.f;
        float confirmCao = 150.f;
        float confirmX = modalX + (modalRong - confirmRong) / 2.f;
        float confirmY = modalY + (modalCao - confirmCao) / 2.f;
        VeKhung(window, confirmX, confirmY, confirmRong, confirmCao, "XAC NHAN THANH LY", font);
        sf::Text confirmText = TaoVanBan(font, "Ban co chac muon thanh ly ma sach:\n" + currentState.maSachCanThanhLy + "?", FONT_SIZE_BINH_THUONG, MAU_LOI);
        confirmText.setPosition(confirmX + paddingNoiBo, confirmY + 45.f);
        window.draw(confirmText);
        float confirmBtnY = confirmY + confirmCao - 50.f;
        float confirmBtnWidth = (confirmRong - 3 * paddingNoiBo) / 2.f;
        TaoNut(font, NUT_XAC_NHAN_THANHLY, confirmX + paddingNoiBo, confirmBtnY, confirmBtnWidth, 40.f, "XAC NHAN", MAU_LOI, MAU_CHU_NUT);
        TaoNut(font, NUT_HUY_THANHLY, confirmX + 2 * paddingNoiBo + confirmBtnWidth, confirmBtnY, confirmBtnWidth, 40.f, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
    }
    
    // Ve Hop Thoai Xac Nhan Xoa Ban Sao
    if (currentState.xacNhanXoaBanSao) {
        sf::RectangleShape confirmOverlay(sf::Vector2f(modalRong, modalCao));
        confirmOverlay.setPosition(modalX, modalY);
        confirmOverlay.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(confirmOverlay);
        float confirmRong = 400.f;
        float confirmCao = 150.f;
        float confirmX = modalX + (modalRong - confirmRong) / 2.f;
        float confirmY = modalY + (modalCao - confirmCao) / 2.f;
        VeKhung(window, confirmX, confirmY, confirmRong, confirmCao, "XAC NHAN XOA BAN SAO", font);
        sf::Text confirmText = TaoVanBan(font, "Ban co chac muon xoa ban sao:\n" + currentState.maSachCanXoa + "?", FONT_SIZE_BINH_THUONG, MAU_LOI);
        confirmText.setPosition(confirmX + paddingNoiBo, confirmY + 45.f);
        window.draw(confirmText);
        float confirmBtnY = confirmY + confirmCao - 50.f;
        float confirmBtnWidth = (confirmRong - 3 * paddingNoiBo) / 2.f;
        TaoNut(font, NUT_XAC_NHAN_XOA_BANSAO, confirmX + paddingNoiBo, confirmBtnY, confirmBtnWidth, 40.f, "XAC NHAN", MAU_LOI, MAU_CHU_NUT);
        TaoNut(font, NUT_HUY_XOA_BANSAO, confirmX + 2 * paddingNoiBo + confirmBtnWidth, confirmBtnY, confirmBtnWidth, 40.f, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
    }
    
    // Ve cac nut dieu khien phia duoi modal (Dong + Phan trang)
    float bottomY = modalY + modalCao - 50.f;
    
    // TOI UU: Tan dung field tongBanSao thay vi duyet DSLK dem lai
    int tongBanSaoModal = dauSach->tongBanSao;                         // O(1) - da co san
    int tongSoTrangModal = (tongBanSaoModal + SO_SACH_MOI_TRANG_MODAL - 1) / SO_SACH_MOI_TRANG_MODAL;
    if (tongSoTrangModal < 1) tongSoTrangModal = 1;
    
    // Nut phan trang (neu co nhieu hon 1 trang)
    if (tongSoTrangModal > 1) {
        TaoNut(font, NUT_MODAL_TRANG_TRUOC, modalX + paddingNoiBo, bottomY, 80.f, 40.f, "<< Truoc", MAU_NEN_NUT, MAU_CHU_NUT);
        TaoNut(font, NUT_MODAL_TRANG_SAU, modalX + paddingNoiBo + 90.f, bottomY, 80.f, 40.f, "Sau >>", MAU_NEN_NUT, MAU_CHU_NUT);
        
        // Hien thi "Trang X / Y"
        sf::Text pageInfoModal = TaoVanBan(font, "Trang " + std::to_string(currentState.trangModal) + " / " + std::to_string(tongSoTrangModal), FONT_SIZE_NHO, MAU_CHU);
        pageInfoModal.setPosition(modalX + paddingNoiBo + 180.f, bottomY + 10.f);
        window.draw(pageInfoModal);
    }
    
    // Nut Dong (ben phai)
    TaoNut(font, NUT_DONG_MODAL_CHITIET, modalX + modalRong - 120.f - paddingNoiBo, bottomY, 120.f, 40.f, "DONG", MAU_LOI, MAU_CHU_NUT);
}

// Ham Xu Ly Su Kien CHINH
void XuLySuKienManHinhSach(sf::RenderWindow &window, sf::Event event) {
    (void)window; // danh dau khong dung bien window

    // Xu ly cuon chuot cho che do xem The Loai
    if (state.cheDoXemHienTai == XEM_THEO_THE_LOAI && !state.hienThiModalBanSao && !state.hienThiModalThemBS) { // Chi cuon khi o che do TL va khong co modal
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                // Dieu chinh scrollOffsetYTheLoai dua tren delta, 30.f la toc do cuon
                state.scrollOffsetYTheLoai -= event.mouseWheelScroll.delta * 30.f;

                // Gioi han lai scroll sau khi cuon
                float maxScroll = 0.f;
                if (state.totalContentHeightTheLoai > (CHIEU_CAO - (PADDING * 5.5f) - (NUT_CAO * 2) - (BANG_Y + 90.f))) { // Recalculate contentHeight equivalent
                    maxScroll = state.totalContentHeightTheLoai - (CHIEU_CAO - (PADDING * 5.5f) - (NUT_CAO * 2) - (BANG_Y + 90.f));
                }
                if (state.scrollOffsetYTheLoai < 0.f)
                    state.scrollOffsetYTheLoai = 0.f;
                if (state.scrollOffsetYTheLoai > maxScroll)
                    state.scrollOffsetYTheLoai = maxScroll;

                return; // Da xu ly scroll, khong can lam gi them voi su kien nay
            }
        }
    }

    // chan tat ca su kien neu modal dang hien
    if (state.hienThiModalBanSao || state.hienThiModalThemBS) {
        if (state.hienThiModalThemBS) { // Xu ly rieng cho Modal Them Ban Sao
            if (event.type == sf::Event::TextEntered || 
                (event.type == sf::Event::KeyPressed && 
                 (event.key.code == sf::Keyboard::Backspace || 
                  event.key.code == sf::Keyboard::Tab ||
                  event.key.code == sf::Keyboard::Z))) { // Them Z de xu ly Ctrl+Z
                // Xu ly input So Luong - goi XuLyTextInput de co Ctrl+Z
                if (inputHoatDong == INPUT_SO_LUONG_THEM) {
                    XuLyTextInput(event, state);
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    MaUI elementNhan = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);
                    if (elementNhan == NUT_XAC_NHAN_THEM_BS) {
                        ThucHienThemBanSao(state);
                    }
                    else if (elementNhan == NUT_HUY_THEM_BS) {
                        state.hienThiModalThemBS = false;
                        state.soLuongBanSaoCanThemStr = "";
                        inputHoatDong = KHONG_XAC_DINH;
                        CapNhatThongBaoSFML("Da huy them ban sao.", 0);
                    }
                    else if (elementNhan == INPUT_SO_LUONG_THEM) {
                        inputHoatDong = INPUT_SO_LUONG_THEM;
                    }
                    else {
                        inputHoatDong = KHONG_XAC_DINH;
                    }
                }
            }
            return; // Chan cac su kien khac
        }
        else if (state.hienThiModalBanSao) { // Xu ly rieng cho Modal Chi Tiet
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    MaUI elementNhan = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);

                    // Xu ly hop thoai xac nhan THANH LY
                    if (state.xacNhanThanhLy) {
                        if (elementNhan == NUT_XAC_NHAN_THANHLY) {
                            ThucHienThanhLySach(state, state.maSachCanThanhLy);
                            state.xacNhanThanhLy = false;
                            state.maSachCanThanhLy = "";
                        }
                        else if (elementNhan == NUT_HUY_THANHLY) {
                            state.xacNhanThanhLy = false;
                            state.maSachCanThanhLy = "";
                            CapNhatThongBaoSFML("Da huy thanh ly.", 0);
                        }
                        return;
                    }
                    
                    // Xu ly hop thoai xac nhan XOA BAN SAO
                    if (state.xacNhanXoaBanSao) {
                        if (elementNhan == NUT_XAC_NHAN_XOA_BANSAO) {
                            ThucHienXoaBanSao(state, state.maSachCanXoa);
                            state.xacNhanXoaBanSao = false;
                            state.maSachCanXoa = "";
                        }
                        else if (elementNhan == NUT_HUY_XOA_BANSAO) {
                            state.xacNhanXoaBanSao = false;
                            state.maSachCanXoa = "";
                            CapNhatThongBaoSFML("Da huy xoa ban sao.", 0);
                        }
                        return;
                    }

                    if (elementNhan == NUT_DONG_MODAL_CHITIET) {
                        state.hienThiModalBanSao = false;
                        state.idModalSachDuocChon = "";
                        state.trangModal = 1;
                        return;
                    }
                    
                    // Xu ly phan trang modal
                    if (elementNhan == NUT_MODAL_TRANG_TRUOC) {
                        if (state.trangModal > 1) {
                            state.trangModal--;
                        }
                        return;
                    }
                    if (elementNhan == NUT_MODAL_TRANG_SAU) {
                        // Tinh tong so trang (co bao ve vong lap)
                        extern PTRDS dsDauSach[];
                        extern int soLuongDauSach;
                        PTRDS dauSachCheck = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, state.idModalSachDuocChon);
                        if (dauSachCheck) {
                            int tongBS = 0;
                            PTRDMS pC = dauSachCheck->dms;
                            int demBV = 0;
                            const int MAX_DEM_BV = 1000;
                            while (pC && demBV < MAX_DEM_BV) { 
                                tongBS++; 
                                pC = pC->next; 
                                demBV++;
                            }
                            int tongTrang = (tongBS + SO_SACH_MOI_TRANG_MODAL - 1) / SO_SACH_MOI_TRANG_MODAL;
                            if (state.trangModal < tongTrang) {
                                state.trangModal++;
                            }
                        }
                        return;
                    }

                    // Kiem tra click vao nut Thanh Ly hoac Xoa
                    for (int i = 0; i < state.soLuongNutModal; ++i) {
                        // Nut Thanh Ly
                        if (state.cacNutTrongModal[i].khuVucNhanThanhLy.contains(event.mouseButton.x, event.mouseButton.y)) {
                            state.xacNhanThanhLy = true;
                            state.maSachCanThanhLy = state.cacNutTrongModal[i].maSach;
                            return;
                        }
                        // Nut Xoa
                        if (state.cacNutTrongModal[i].khuVucNhanXoa.contains(event.mouseButton.x, event.mouseButton.y)) {
                            state.xacNhanXoaBanSao = true;
                            state.maSachCanXoa = state.cacNutTrongModal[i].maSach;
                            return;
                        }
                    }
                }
            }
            return; // Chan cac su kien khac
        }
    }

    // Code xu ly binh thuong (khi modal khong hien)
    // Xu ly nhap lieu van ban (go phim, xoa, undo...)
    if (event.type == sf::Event::TextEntered || 
        (event.type == sf::Event::KeyPressed && 
         (event.key.code == sf::Keyboard::Backspace || 
          event.key.code == sf::Keyboard::Enter || 
          event.key.code == sf::Keyboard::Tab ||
          event.key.code == sf::Keyboard::Z))) { // Them Z de xu ly Ctrl+Z
        if (inputHoatDong != KHONG_XAC_DINH) {
            XuLyTextInput(event, state);
        }
    }

    // Xu ly nhan chuot
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            MaUI elementNhan = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);

            // Kiem tra click vao O NHAP LIEU de active
            bool clickedInput = false;
            if (elementNhan == INPUT_TIM_SACH ||
                elementNhan == INPUT_ISBN ||
                elementNhan == INPUT_TEN_SACH ||
                elementNhan == INPUT_SO_TRANG ||
                elementNhan == INPUT_TAC_GIA ||
                elementNhan == INPUT_NAM_XB ||
                elementNhan == INPUT_THE_LOAI ||
                elementNhan == INPUT_SO_LUONG ||
                elementNhan == INPUT_SO_LUONG_THEM) {
                if (!(state.dangSua && elementNhan == INPUT_ISBN)) {
                    // Neu chuyen sang input khac, reset stack undo
                    if (inputHoatDong != elementNhan) {
                        // Luu trang thai cu vao stack truoc khi chuyen (neu co)
                        if (inputHoatDong != KHONG_XAC_DINH && state.stackDaKhoiTao) {
                            // Khong can lam gi, vi stack se duoc reset khi chuyen input
                        }
                        // Reset stack cho input moi
                        KhoiTaoStack(&state.undoStack);
                        state.stackDaKhoiTao = false; // Danh dau chua khoi tao stack cho input moi
                        inputHoatDong = elementNhan;
                        CapNhatThongBaoSFML("", 0);
                    }
                    clickedInput = true;
                }
            }

            // Neu click ra ngoai o input thi deactive
            if (!clickedInput && inputHoatDong != KHONG_XAC_DINH) {
                inputHoatDong = KHONG_XAC_DINH;
                KhoiTaoStack(&state.undoStack);
                state.stackDaKhoiTao = false; // Reset flag
                CapNhatThongBaoSFML("", 0);
            }

            // Xu ly click vao NUT (neu khong phai click vao input)
            if (!clickedInput) {
                switch (elementNhan) {
                // Nut Quay Ve Menu hoac Huy Sua
                case NUT_BACK:
                    if (state.dangSua) { // Neu dang sua thi Huy Sua
                        state.dangSua = false;
                        XoaFormNhapLieuSFML(state);
                        CapNhatThongBaoSFML("Da huy hieu chinh.", 0);
                    }
                    else {                                // Neu khong thi Quay Ve Menu
                        manHinhHienTai = MENU_CHINH; // Chuyen man hinh
                        // Reset cac trang thai cua man hinh Sach
                        state.dangSua = false;
                        state.xacNhanXoa = false;
                        inputHoatDong = KHONG_XAC_DINH;
                        state.isbnSachDuocChon = "";
                        state.cheDoXemHienTai = XEM_TIM_KIEM; // Reset che do xem
                        state.scrollOffsetYTheLoai = 0.f;     // Reset vi tri cuon
                        CapNhatThongBaoSFML("", 0);     // Xoa thong bao
                    }
                    break; // Ket thuc case NUT_BACK
                // Nut Tim Kiem
                case NUT_TIM:
                    inputHoatDong = KHONG_XAC_DINH;
                    state.cheDoXemHienTai = XEM_TIM_KIEM; // Nhan Tim thi luon chuyen ve che do Tim Kiem
                    state.scrollOffsetYTheLoai = 0.f;
                    ThucHienTimKiemNoiBo(state);
                    break;
                // Nut Xoa Tim Kiem (Reset)
                case NUT_XOA_TIM:
                    state.chuoiTimKiem = "";
                    inputHoatDong = KHONG_XAC_DINH;
                    state.cheDoXemHienTai = XEM_TIM_KIEM; // Reset cung chuyen ve che do Tim Kiem
                    state.scrollOffsetYTheLoai = 0.f;
                    ThucHienTimKiemNoiBo(state);
                    break;
                // Nut Them Sach / Luu Thay Doi
                case NUT_THEM_SACH:
                    inputHoatDong = KHONG_XAC_DINH;
                    ThucHienThemHoacSuaSachSFML(state);
                    break;

                case NUT_CHUYEN_CHE_DO_XEM:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM) {
                        state.cheDoXemHienTai = XEM_THEO_THE_LOAI;
                        state.scrollOffsetYTheLoai = 0.f;
                        // Reset trang thai khong can thiet cho che do xem the loai
                        state.isbnSachDuocChon = "";
                        state.dangSua = false;
                        state.xacNhanXoa = false;
                        inputHoatDong = KHONG_XAC_DINH;
                        CapNhatDuLieuXemTheoTheLoai(state);  // TOI UU: Cap nhat du lieu 1 lan
                        CapNhatThongBaoSFML("Dang xem theo the loai.", 0);
                    }
                    else {
                        state.cheDoXemHienTai = XEM_TIM_KIEM;
                        state.scrollOffsetYTheLoai = 0.f;
                        ThucHienTimKiemNoiBo(state); // Tai lai danh sach tim kiem/tat ca
                        // Thong bao da duoc cap nhat trong ThucHienTimKiemNoiBo
                    }
                    break;

                case NUT_TRANG_TRUOC:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM && state.trangHienTai > 1)
                        state.trangHienTai--;
                    break;
                case NUT_TRANG_SAU:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM && state.trangHienTai < state.tongSoTrang)
                        state.trangHienTai++;
                    break;
                case NUT_THEM_BAN_SAO:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM && !state.isbnSachDuocChon.empty()) {
                        state.hienThiModalThemBS = true;
                        state.soLuongBanSaoCanThemStr = "";
                        inputHoatDong = INPUT_SO_LUONG_THEM;
                        CapNhatThongBaoSFML("Nhap so luong ban sao can them.", 0);
                    }
                    else if (state.cheDoXemHienTai == XEM_THEO_THE_LOAI) {
                        CapNhatThongBaoSFML("Khong dung duoc o che do xem The Loai!", 1);
                    }
                    else {
                        CapNhatThongBaoSFML("Loi: Vui long chon sach truoc!", 1);
                    }
                    break;
                case NUT_CHI_TIET_SACH:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM && !state.isbnSachDuocChon.empty()) {
                        state.hienThiModalBanSao = true;
                        state.idModalSachDuocChon = state.isbnSachDuocChon;
                        state.trangModal = 1;
                    }
                    else if (state.cheDoXemHienTai == XEM_THEO_THE_LOAI) {
                        CapNhatThongBaoSFML("Khong dung duoc o che do xem The Loai!", 1);
                    }
                    else {
                        CapNhatThongBaoSFML("Loi: Vui long chon sach truoc!", 1);
                    }
                    break;
                case NUT_SUA:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM && !state.isbnSachDuocChon.empty()) {
                        DienFormVoiSachDuocChon(state);
                        state.dangSua = true;
                        state.xacNhanXoa = false;
                        inputHoatDong = INPUT_TEN_SACH;
                        CapNhatThongBaoSFML("Dang hieu chinh sach...", 0);
                    }
                    else if (state.cheDoXemHienTai == XEM_THEO_THE_LOAI) {
                        CapNhatThongBaoSFML("Khong dung duoc o che do xem The Loai!", 1);
                    }
                    else
                        CapNhatThongBaoSFML("Vui long chon mot dau sach de sua!", 1);
                    break;
                case NUT_XOA:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM) { // chi kiem tra xoa o che do tim kiem
                        if (state.isbnSachDuocChon.empty()) {
                            CapNhatThongBaoSFML("Loi: Vui long chon mot dau sach de xoa!", 1);
                        }
                        else {
                            extern PTRDS dsDauSach[];
                            extern int soLuongDauSach;
                            PTRDS sachCanXoa = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, state.isbnSachDuocChon);

                            if (sachCanXoa && sachCanXoa->dms != nullptr) { // kiem tra con ban sao
                                // cat ngan ten sach neu qua dai
                                std::string tenNgan = CatChuoiVoiDauCham(sachCanXoa->tenSach, 30);
                                CapNhatThongBaoSFML("Khong the xoa '" + tenNgan + "' (con ban sao)!", 1);
                            }
                            else if (sachCanXoa) {
                                state.xacNhanXoa = true; // moi hien xac nhan
                                state.dangSua = false;
                                inputHoatDong = KHONG_XAC_DINH;
                                CapNhatThongBaoSFML("", 0);
                            }
                            else {
                                CapNhatThongBaoSFML("Loi: Khong tim thay dau sach da chon de xoa!", 1);
                            }
                        }
                    }
                    else {
                        CapNhatThongBaoSFML("Chuc nang nay khong dung duoc o che do xem theo the loai.", 1);
                    }
                    break;
                case NUT_XAC_NHAN_XOA: // Giu nguyen, chi goi khi xacNhanXoa=true
                    if (state.xacNhanXoa && !state.isbnSachDuocChon.empty())
                        ThucHienXoaSachSFML(state);
                    state.xacNhanXoa = false;
                    break;
                case NUT_HUY_XOA: // Giu nguyen
                    state.xacNhanXoa = false;
                    CapNhatThongBaoSFML("Da huy thao tac xoa.", 0);
                    break;
                case NUT_HUY_CHON:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM) { // chi huy chon o che do tim kiem
                        state.isbnSachDuocChon = "";
                        state.dangSua = false;
                        state.xacNhanXoa = false;
                        XoaFormNhapLieuSFML(state);
                        CapNhatThongBaoSFML("Da huy chon.", 0);
                    }
                    break;
                case HANG_SACH:
                    if (state.cheDoXemHienTai == XEM_TIM_KIEM) { // chi xu ly double click o che do tim kiem
                        float contentY = BANG_Y + 90 + 35;
                        float rowHeight = 30.f;
                        int startIndex = (state.trangHienTai - 1) * SACH_MOI_TRANG;
                        int rowIndex = static_cast<int>((event.mouseButton.y - contentY) / rowHeight);
                        int actualIndex = startIndex + rowIndex;

                        if (actualIndex >= 0 && actualIndex < state.soLuongKetQuaTimKiem && state.ketQuaTimKiem[actualIndex].sach) {
                            state.isbnRowClicked = state.ketQuaTimKiem[actualIndex].sach->ISBN;
                        }
                        else {
                            state.isbnRowClicked = "";
                        }

                        if (state.isbnRowClicked.empty())
                            break;
                        float thoiGianTroiQua = state.doubleClickClock.getElapsedTime().asSeconds();
                        if (thoiGianTroiQua < state.THOI_GIAN_DOUBLE_CLICK && state.isbnRowClicked == state.isbnClickCuoi) {
                            state.isbnSachDuocChon = state.isbnRowClicked;
                            state.dangSua = false;
                            state.xacNhanXoa = false;
                            // cat ngan ten sach
                            std::string tenSachHienThi = CatChuoiVoiDauCham(state.ketQuaTimKiem[actualIndex].sach->tenSach, 35);
                            CapNhatThongBaoSFML("Da chon: " + tenSachHienThi, 0);
                            inputHoatDong = KHONG_XAC_DINH;
                            XoaFormNhapLieuSFML(state);
                            state.isbnClickCuoi = "";
                        }
                        else {
                            state.isbnClickCuoi = state.isbnRowClicked;
                            state.doubleClickClock.restart();
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
    // Xu ly nhan phim ESCAPE (thoat input, huy sua/xoa, quay ve menu)
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        if (inputHoatDong != KHONG_XAC_DINH) {
            inputHoatDong = KHONG_XAC_DINH;
        }
        else if (state.dangSua) {
            state.dangSua = false;
            XoaFormNhapLieuSFML(state);
            CapNhatThongBaoSFML("Da huy hieu chinh.", 0);
        }
        else if (state.xacNhanXoa) {
            state.xacNhanXoa = false;
            CapNhatThongBaoSFML("Da huy thao tac xoa.", 0);
        }
        else {
            manHinhHienTai = MENU_CHINH;
            state.cheDoXemHienTai = XEM_TIM_KIEM; // Reset che do xem khi thoat
            state.scrollOffsetYTheLoai = 0.f;
            CapNhatThongBaoSFML("", 0);
        }
    }
}


// Ham xu ly nhap lieu text 
static void XuLyTextInput(sf::Event event, SachState& currentState) {
    // Kiem tra xem co input nao dang active khong
    if (inputHoatDong == KHONG_XAC_DINH) return;
    // Khong xu ly Tab neu la o nhap so luong trong modal Them BS
    if(inputHoatDong == INPUT_SO_LUONG_THEM && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) return;

    // Khoi tao stack lan dau (neu chua)
    if (!currentState.stackDaKhoiTao) {
        KhoiTaoStack(&currentState.undoStack);
        currentState.stackDaKhoiTao = true;
    }

    std::string* targetString = nullptr; // Con tro den chuoi can sua
    int maxLen = 0;
    bool chiNhanSo = false;
    std::string fieldName = "";

    // Gan con tro chuoi vao input dang active
    switch (inputHoatDong) {
        case INPUT_TIM_SACH: targetString = &currentState.chuoiTimKiem; maxLen = 50; fieldName = "Tim kiem"; break;
        case INPUT_ISBN:     targetString = &currentState.chuoiISBN; maxLen = 13; chiNhanSo = true; fieldName = "ISBN"; break;
        case INPUT_TEN_SACH: targetString = &currentState.chuoiTenSach; maxLen = MAX_TEN_SACH; fieldName = "Ten sach"; break;
        case INPUT_SO_TRANG: targetString = &currentState.chuoiSoTrang; maxLen = 5; chiNhanSo = true; fieldName = "So trang"; break;
        case INPUT_TAC_GIA:  targetString = &currentState.chuoiTacGia; maxLen = MAX_TAC_GIA; fieldName = "Tac gia"; break;
        case INPUT_NAM_XB:   targetString = &currentState.chuoiNamXB; maxLen = 4; chiNhanSo = true; fieldName = "Nam XB"; break;
        case INPUT_THE_LOAI: targetString = &currentState.chuoiTheLoai; maxLen = MAX_THE_LOAI; fieldName = "The loai"; break;
        case INPUT_SO_LUONG: targetString = &currentState.chuoiSoLuong; maxLen = 3; chiNhanSo = true; fieldName = "So luong"; break;
        case INPUT_SO_LUONG_THEM: targetString = &currentState.soLuongBanSaoCanThemStr; maxLen = 3; chiNhanSo = true; fieldName = "So luong them"; break;
        default: return; // Thoat neu khong phai input hop le
    }

    bool changed = false; // Co de kiem tra xem noi dung co thay doi khong
    std::string currentInputContent = (targetString != nullptr) ? *targetString : ""; // Luu noi dung hien tai TRUOC KHI thay doi

    // Xu ly Ctrl + Z (Undo) - XU LY TRUOC cac event khac
    // Dung sf::Keyboard::isKeyPressed vi event.key.control khong on dinh tren Windows
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z && 
        (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
        
        if (!KiemTraStackRong(&currentState.undoStack) && targetString != nullptr) {
            // Pop trang thai cu va gan lai
            std::string previousState = PopStack(&currentState.undoStack);
            *targetString = previousState;
            changed = true; // Danh dau da thay doi de chay validation
            
            // Xoa thong bao loi (neu co)
            if (loaiThongBao == 1) {
                CapNhatThongBaoSFML("", 0);
            }
        } else {
            // Stack rong, khong co gi de undo
            if (targetString != nullptr) {
                CapNhatThongBaoSFML("Khong co gi de undo!", 0);
            }
        }
        return; // Return sau khi xu ly de tranh xu ly tiep
    }

    // Xu ly Enter cho INPUT_SO_LUONG_THEM (xac nhan them ban sao)
    if (event.type == sf::Event::KeyPressed && inputHoatDong == INPUT_SO_LUONG_THEM) {
        if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Enter) {
            ThucHienThemBanSao(currentState); // currentState o day la tham so SachState&
            return;
        }
    }

    // Xu ly go phim (CHI khi khong phai Ctrl+Z da duoc xu ly)
    if (event.type == sf::Event::TextEntered && !(event.type == sf::Event::KeyPressed && event.key.control && event.key.code == sf::Keyboard::Z)) {
        // Chi lay ky tu ASCII, khong lay Backspace, Enter, Tab
        if (event.text.unicode < 128 && event.text.unicode != 8 && event.text.unicode != 13 && event.text.unicode != 9) {
            char enteredChar = static_cast<char>(event.text.unicode);
            if (targetString->length() < static_cast<size_t>(maxLen)) { // Kiem tra do dai toi da
                bool allowChar = false; // Co cho phep them ky tu nay khong
                std::string loiNhap = ""; // Luu thong bao loi neu co
                
                if (chiNhanSo) { // Neu la o chi nhan so
                    if (enteredChar >= '0' && enteredChar <= '9') {
                        allowChar = true;
                    } else if (enteredChar >= 32) { // Ky tu co the hien
                        loiNhap = "Loi: " + fieldName + " chi duoc nhap so (0-9)!";
                    }
                } else { // Neu la o nhan chu va so
                    // Cho phep chu, so, khoang trang va 1 so ky tu dac biet
                    if (isalnum(enteredChar) || enteredChar == ' ' || enteredChar == '+' || enteredChar == '#' || enteredChar == '-' || enteredChar == '.' || enteredChar == '_') {
                        // Ky tu dau tien phai la chu cai
                        if (targetString->empty() && !isalpha(enteredChar)) {
                            loiNhap = "Loi: " + fieldName + " phai bat dau bang chu cai!";
                        } 
                        // Khong cho phep nhieu khoang trang lien tiep
                        else if (enteredChar == ' ' && !targetString->empty() && targetString->back() == ' ') {
                            loiNhap = "Loi: Khong duoc nhap nhieu khoang trang lien tiep!";
                        }
                        else {
                            allowChar = true;
                        }
                    } else if (enteredChar >= 32) { // Ky tu khong hop le
                        loiNhap = "Loi: Ky tu '" + std::string(1, enteredChar) + "' khong duoc phep nhap!";
                    }
                }
                
                // Hien thi loi neu co
                if (!loiNhap.empty()) {
                    CapNhatThongBaoSFML(loiNhap, 1);
                }
                
                // Neu ky tu hop le
                if (allowChar) {
                    // Dam bao stack da duoc khoi tao
                    if (!currentState.stackDaKhoiTao) {
                        KhoiTaoStack(&currentState.undoStack);
                        currentState.stackDaKhoiTao = true;
                    }
                    
                    // Push trang thai cu vao stack TRUOC KHI them ky tu moi
                    PushStack(&currentState.undoStack, currentInputContent);
                    
                    // Them ky tu vao chuoi
                    *targetString += enteredChar;
                    changed = true; // Danh dau da thay doi
                    
                    // Xoa loi neu co (validation se chay lai o phan duoi)
                    if(loaiThongBao == 1) CapNhatThongBaoSFML("", 0);
                }
            } else if (event.text.unicode >= 32 && event.text.unicode < 128) { // Vuot qua gioi han
                CapNhatThongBaoSFML("Loi: " + fieldName + " da dat gioi han " + std::to_string(maxLen) + " ky tu!\nHien tai: " + std::to_string(targetString->length()) + " ky tu. Vui long rut ngan.", 1);
            }
        }
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Backspace && 
               !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) { // Xu ly Backspace (khong phai khi giu Ctrl)
        if (!targetString->empty() && targetString != nullptr) {
            // Dam bao stack da duoc khoi tao
            if (!currentState.stackDaKhoiTao) {
                KhoiTaoStack(&currentState.undoStack);
                currentState.stackDaKhoiTao = true;
            }
            
            // Push trang thai CU vao stack TRUOC KHI xoa
            PushStack(&currentState.undoStack, currentInputContent);
            
            // Xoa ky tu cuoi
            targetString->pop_back();
            changed = true; // Danh dau da thay doi
            
            // Xoa loi neu co (validation se chay lai o phan duoi)
            if(loaiThongBao == 1) CapNhatThongBaoSFML("", 0);
        }
    }

    // Kiem tra loi ngay khi dang go (realtime validation)
    // Validate cho TAT CA cac truong (ke ca khi xoa text)
    if (changed) {
        std::string loi = "";
        std::string tempStr = *targetString;
        
        switch (inputHoatDong) {
            case INPUT_TIM_SACH:
                // Khong hien thi loi cho o tim kiem, chi validate format (neu can)
                // Co the them validation sau neu can
                break;
            case INPUT_SO_LUONG_THEM:
                // Validate so luong them ban sao
                if (!tempStr.empty()) {
                    bool isValidNumber = true;
                    for(char c : tempStr) {
                        if (!isdigit(c)) {
                            isValidNumber = false;
                            loi = "Loi: So luong phai la so nguyen!\nKy tu '" + std::string(1, c) + "' khong hop le.";
                            break;
                        }
                    }
                    if (isValidNumber && !tempStr.empty()) {
                        int value;
                        if (ChuyenChuoiThanhSoNguyen(tempStr, value, true)) {
                            if (value <= 0) {
                                loi = "Loi: So luong phai lon hon 0!\nVui long nhap so nguyen duong.\nVi du: 1, 5, 10.";
                            } else if (value > MAX_BAN_SAO) {
                                loi = "Loi: So luong '" + std::to_string(value) + "' vuot qua gioi han!\nSo luong toi da: " + std::to_string(MAX_BAN_SAO) + " ban sao.\nVi du: 1, 5, 10.";
                            }
                        }
                    }
                }
                break;
            case INPUT_ISBN:
                // Realtime validation cho ISBN
                // Luu y: Khi SUA thi ISBN la readonly, nen khong can validate realtime
                // Nhung neu user van go duoc (bug) thi van validate de dam bao
                if (!currentState.dangSua) {
                    // CHI VALIDATE KHI THEM (dangSua = false)
                    loi = KiemTraChuoiRong(tempStr, fieldName);
                    if (loi.empty() && tempStr.length() > 0) {
                        // Check chi chua so
                        for(char c : tempStr) { 
                            if (!isdigit(c)) { 
                                loi = "Loi: ISBN chi duoc chua so (0-9)!\nKy tu '" + std::string(1, c) + "' khong hop le.\nVi du: 9780123456789 hoac 9876047765."; 
                                break; 
                            } 
                        }
                        // Check do dai (10 hoac 13 chu so)
                        if (loi.empty()) {
                            if (tempStr.length() != 10 && tempStr.length() != 13) {
                                loi = "Loi: ISBN phai co 10 hoac 13 chu so!\nHien tai co " + std::to_string(tempStr.length()) + " chu so.\nVi du: 9780123456789 (13 so) hoac 9876047765 (10 so).";
                            }
                        }
                    }
                }
                // Khi SUA (dangSua = true): Khong validate realtime vi ISBN readonly
                break;
            case INPUT_TEN_SACH:
            case INPUT_TAC_GIA:
            case INPUT_THE_LOAI:
                // Validate chuoi text
                loi = KiemTraChuoiVaDodai(tempStr, fieldName, maxLen);
                if (loi.empty() && !tempStr.empty() && !isalpha(tempStr[0])) {
                    loi = "Loi: " + fieldName + " phai bat dau bang chu cai!\nKy tu dau '" + std::string(1, tempStr[0]) + "' khong hop le.\nVi du: 'Tieu De' thay vi '123 Tieu De'.";
                }
                break;
            case INPUT_SO_TRANG:
                // Validate so trang
                if (!tempStr.empty()) {
                    bool isValidNumber = true;
                    for(char c : tempStr) { 
                        if (!isdigit(c)) { 
                            isValidNumber = false;
                            loi = "Loi: So trang phai la so nguyen!\nKy tu '" + std::string(1, c) + "' khong hop le.\nChi duoc nhap so (0-9)."; 
                            break; 
                        } 
                    }
                    if (isValidNumber && !tempStr.empty()) {
                        int value;
                        if (ChuyenChuoiThanhSoNguyen(tempStr, value, true)) {
                            if (value <= 0) {
                                loi = "Loi: So trang phai lon hon 0!\nVui long nhap so nguyen duong.\nVi du: 250, 500, 1000.";
                            } else if (value > MAX_SO_TRANG) {
                                loi = "Loi: So trang '" + std::to_string(value) + "' vuot qua gioi han!\nSo trang toi da: " + std::to_string(MAX_SO_TRANG) + ".\nVi du: 250, 500, 1000.";
                            }
                        } else {
                            loi = "Loi: So trang khong hop le!\nVui long nhap so nguyen dung dinh dang.";
                        }
                    }
                }
                break;
            case INPUT_NAM_XB:
                // Validate nam xuat ban
                if (!tempStr.empty()) {
                    bool isValidNumber = true;
                    for(char c : tempStr) { 
                        if (!isdigit(c)) { 
                            isValidNumber = false;
                            loi = "Loi: Nam xuat ban phai la so!\nKy tu '" + std::string(1, c) + "' khong hop le.\nChi duoc nhap so (0-9)."; 
                            break; 
                        } 
                    }
                    if (isValidNumber && !tempStr.empty()) {
                        int value;
                        if (ChuyenChuoiThanhSoNguyen(tempStr, value, true)) {
                            // Chi validate khi da du 4 chu so
                            if (tempStr.length() == 4) {
                                loi = KiemTraNamXuatBan(value);
                            } else if (tempStr.length() > 4) {
                                loi = "Loi: Nam xuat ban phai co 4 chu so!\nVui long nhap nam hop le.\nVi du: 2020, 2021, 2024.";
                            }
                            // Neu chua du 4 chu so thi khong bao loi (dang nhap)
                        } else {
                            loi = "Loi: Nam xuat ban khong hop le!\nVui long nhap so nguyen 4 chu so.\nVi du: 2020, 2021, 2024.";
                        }
                    }
                }
                break;
            case INPUT_SO_LUONG:
                // Validate so luong ban sao (khi them dau sach)
                if (!tempStr.empty()) {
                    bool isValidNumber = true;
                    for(char c : tempStr) { 
                        if (!isdigit(c)) { 
                            isValidNumber = false;
                            loi = "Loi: So luong phai la so nguyen!\nKy tu '" + std::string(1, c) + "' khong hop le.\nChi duoc nhap so (0-9)."; 
                            break; 
                        } 
                    }
                    if (isValidNumber && !tempStr.empty()) {
                        int value;
                        if (ChuyenChuoiThanhSoNguyen(tempStr, value, true)) {
                            if (value <= 0) {
                                loi = "Loi: So luong phai lon hon 0!\nVui long nhap so nguyen duong.\nVi du: 1, 5, 10 ban sao.";
                            } else if (value > MAX_BAN_SAO) {
                                loi = "Loi: So luong '" + std::to_string(value) + "' vuot qua gioi han!\nSo luong toi da: " + std::to_string(MAX_BAN_SAO) + " ban sao.\nVi du: 1, 5, 10.";
                            }
                        } else {
                            loi = "Loi: So luong khong hop le!\nVui long nhap so nguyen dung dinh dang.";
                        }
                    }
                }
                break;
            default: break;
        }
        
        // Chi hien thi loi cho cac truong khac INPUT_TIM_SACH
        if (inputHoatDong != INPUT_TIM_SACH) {
            if (!loi.empty()) {
                CapNhatThongBaoSFML(loi, 1);
            } else if (loaiThongBao == 1) {
                // Xoa loi neu khong con loi
                CapNhatThongBaoSFML("", 0);
            }
        }
    }

    // Xu ly nhan Enter/Tab de nhay input (khong ap dung cho modal)
    if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Tab) && inputHoatDong != INPUT_SO_LUONG_THEM) {
         std::string loiCuoi = "";
        std::string tempStrCuoi = *targetString;
        tempStrCuoi = CatKhoangTrang(tempStrCuoi);
        switch (inputHoatDong) {
            case INPUT_ISBN:
                loiCuoi = KiemTraChuoiRong(tempStrCuoi, fieldName);
                if (loiCuoi.empty() && !tempStrCuoi.empty()) {
                    std::string isbnChuanTam;
                    std::string loiChuan = ChuanHoaISBNFile(tempStrCuoi, isbnChuanTam);
                    if (!loiChuan.empty()) {
                        loiCuoi = loiChuan;
                    } else if (!currentState.dangSua) {
                        std::string loiTrung = KiemTraTrungISBN(isbnChuanTam);
                        if (!loiTrung.empty()) loiCuoi = loiTrung;
                    }
                }
                break;
            case INPUT_TEN_SACH:
            case INPUT_TAC_GIA:
            case INPUT_THE_LOAI:
                loiCuoi = KiemTraChuoiVaDodai(tempStrCuoi, fieldName, maxLen);
                if (loiCuoi.empty() && !tempStrCuoi.empty() && !isalpha(tempStrCuoi[0])) {
                    loiCuoi = "Loi: " + fieldName + " phai bat dau bang chu cai!\nKy tu dau '" + std::string(1, tempStrCuoi[0]) + "' khong hop le.";
                }
                break;
            case INPUT_SO_TRANG: {
                int v;
                if (!ChuyenChuoiThanhSoNguyen(tempStrCuoi, v, true) || v <= 0 || v > MAX_SO_TRANG) {
                    loiCuoi = "Loi: So trang '" + tempStrCuoi + "' khong hop le!\nSo trang phai tu 1 den " + std::to_string(MAX_SO_TRANG) + ".\nVi du: 250, 500, 1000.";
                }
                break;
            }
            case INPUT_NAM_XB: {
                if (!tempStrCuoi.empty()) {
                    int v;
                    if (ChuyenChuoiThanhSoNguyen(tempStrCuoi, v, true) && tempStrCuoi.length() == 4) {
                        loiCuoi = KiemTraNamXuatBan(v);
                    } else if (tempStrCuoi.length() > 0 && tempStrCuoi.length() != 4) {
                        loiCuoi = "Loi: Nam xuat ban phai co 4 chu so!\nVui long nhap nam hop le.\nVi du: 2020, 2021, 2024.";
                    }
                }
                break;
            }
            case INPUT_SO_LUONG: {
                int v;
                if (!ChuyenChuoiThanhSoNguyen(tempStrCuoi, v, true) || v <= 0 || v > MAX_BAN_SAO) {
                    loiCuoi = "Loi: So luong '" + tempStrCuoi + "' khong hop le!\nSo luong phai tu 1 den " + std::to_string(MAX_BAN_SAO) + " ban sao.\nVi du: 1, 5, 10.";
                }
                break;
            }
            default: break;
        }

        if (loiCuoi.empty()) {
            KhoiTaoStack(&currentState.undoStack); // Reset Stack khi chuyen input
            currentState.stackDaKhoiTao = false; // Reset flag
            if (inputHoatDong >= INPUT_TIM_SACH && inputHoatDong < INPUT_SO_LUONG) {
                inputHoatDong = static_cast<MaUI>(inputHoatDong + 1);
                CapNhatThongBaoSFML("", 0);
            } else if (inputHoatDong == INPUT_SO_LUONG) {
                inputHoatDong = KHONG_XAC_DINH;
                if (event.key.code == sf::Keyboard::Enter) ThucHienThemHoacSuaSachSFML(currentState);
            } else {
                inputHoatDong = KHONG_XAC_DINH;
            }
        } else {
            CapNhatThongBaoSFML(loiCuoi, 1);
        }
    }
}


// Cap nhat so trang hien thi
static void CapNhatPhanTrangSFML(SachState& currentState) {
    int totalItems = currentState.soLuongKetQuaTimKiem;
    currentState.tongSoTrang = (totalItems + SACH_MOI_TRANG - 1) / SACH_MOI_TRANG;
    if (currentState.tongSoTrang == 0)
        currentState.tongSoTrang = 1; // It nhat 1 trang
    if (currentState.trangHienTai > currentState.tongSoTrang)
        currentState.trangHienTai = currentState.tongSoTrang; // Khong vuot qua trang cuoi
}

// Goi backend de tim kiem/lay tat ca sach
static void ThucHienTimKiemNoiBo(SachState& currentState) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    currentState.soLuongKetQuaTimKiem = 0;                               // Reset ket qua cu
    std::string tuKhoa = ChuanHoaKhoangTrang(currentState.chuoiTimKiem); // Lay tu khoa tu input
    
    if (tuKhoa.empty()) { // Neu khong nhap gi -> Hien thi tat ca
        for (int i = 0; i < soLuongDauSach; ++i) {
            if (currentState.soLuongKetQuaTimKiem < MAX_DAUSACH) {
                currentState.ketQuaTimKiem[currentState.soLuongKetQuaTimKiem].sach = dsDauSach[i];
                currentState.ketQuaTimKiem[currentState.soLuongKetQuaTimKiem].loaiKhop = 0;
                currentState.soLuongKetQuaTimKiem++;
            }
            else
                break; // Tranh vuot qua mang ket qua
        }
        
        // Sap xep tat ca sach theo ten (A->Z)
        if (currentState.soLuongKetQuaTimKiem > 1) {
            PTRDS tempArr[MAX_DAUSACH];
            for (int i = 0; i < currentState.soLuongKetQuaTimKiem; ++i) {
                tempArr[i] = currentState.ketQuaTimKiem[i].sach;
            }
            sapXepDauSachTheoTen(tempArr, 0, currentState.soLuongKetQuaTimKiem - 1);
            for (int i = 0; i < currentState.soLuongKetQuaTimKiem; ++i) {
                currentState.ketQuaTimKiem[i].sach = tempArr[i];
            }
        }
    }
    else { // Neu co tu khoa -> Goi ham tim kiem logic
        currentState.soLuongKetQuaTimKiem = timKiemLogic(dsDauSach, soLuongDauSach, tuKhoa, currentState.ketQuaTimKiem);
    }
    currentState.trangHienTai = 1;       // Luon ve trang 1 sau khi tim kiem
    CapNhatPhanTrangSFML(currentState); // Tinh lai tong so trang
    // Reset trang thai sau tim kiem
    currentState.isbnSachDuocChon = "";
    currentState.dangSua = false;
    currentState.xacNhanXoa = false;

    // Chan thong bao tim kiem (de giu thong bao "Them thanh cong")
    if (currentState.vuaThucHienThanhCong == false) {
        if (currentState.soLuongKetQuaTimKiem > 0 || tuKhoa.empty()) {
            if (tuKhoa.empty() && soLuongDauSach > 0)
                CapNhatThongBaoSFML("Hien thi tat ca " + std::to_string(currentState.soLuongKetQuaTimKiem) + " dau sach.", 0);
            else if (currentState.soLuongKetQuaTimKiem > 0)
                CapNhatThongBaoSFML("Tim thay " + std::to_string(currentState.soLuongKetQuaTimKiem) + " ket qua.", 0);
            else if (soLuongDauSach == 0 && tuKhoa.empty())
                CapNhatThongBaoSFML("Danh sach rong.", 0);
            else
                CapNhatThongBaoSFML("Khong tim thay ket qua cho '" + tuKhoa + "'.", 1);
        }
        else {
            CapNhatThongBaoSFML("Khong tim thay ket qua cho '" + tuKhoa + "'.", 1);
        }
    }
    currentState.vuaThucHienThanhCong = false; // Reset co lai
}

// Ham khoi tao (public) - Goi khi chuyen sang man hinh Sach
void KhoiTaoManHinhSach() {
    ThucHienTimKiemNoiBo(state); // Goi ham tim kiem noi bo de load danh sach
}

// Xoa trang noi dung form
static void XoaFormNhapLieuSFML(SachState& currentState) {
    currentState.chuoiISBN = "";
    currentState.chuoiTenSach = "";
    currentState.chuoiSoTrang = "";
    currentState.chuoiTacGia = "";
    currentState.chuoiNamXB = "";
    currentState.chuoiTheLoai = "";
    currentState.chuoiSoLuong = "";
}

// Dien du lieu sach da chon vao form (khi nhan Sua)
static void DienFormVoiSachDuocChon(SachState& currentState) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    PTRDS sach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, currentState.isbnSachDuocChon);
    if (sach) {
        currentState.chuoiISBN = sach->ISBN;
        currentState.chuoiTenSach = sach->tenSach;
        currentState.chuoiSoTrang = std::to_string(sach->soTrang);
        currentState.chuoiTacGia = sach->tacGia;
        currentState.chuoiNamXB = std::to_string(sach->namXuatBan);
        currentState.chuoiTheLoai = sach->theLoai;
    }
    else {
        CapNhatThongBaoSFML("Loi: Khong tim thay sach de sua!", 1);
        XoaFormNhapLieuSFML(currentState);
        currentState.isbnSachDuocChon = ""; // Huy chon neu loi
    }
}

// Goi backend de Them Dau Sach / Cap Nhat Dau Sach
static void ThucHienThemHoacSuaSachSFML(SachState& currentState) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    extern bool duLieuDaThayDoi;
    std::string loi;
    std::string isbnChuan = currentState.chuoiISBN;
    int soTrang = 0;
    int namXB = 0;
    int soLuong = 0;

    // Kiem tra loi chung cho ca Them va Sua
    
    // Cat khoang trang dau/cuoi cho cac truong text
    currentState.chuoiTenSach = CatKhoangTrang(currentState.chuoiTenSach);
    currentState.chuoiTacGia = CatKhoangTrang(currentState.chuoiTacGia);
    currentState.chuoiTheLoai = CatKhoangTrang(currentState.chuoiTheLoai);
    
    loi = KiemTraChuoiVaDodai(currentState.chuoiTenSach, "Ten sach", MAX_TEN_SACH);
    if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        inputHoatDong = INPUT_TEN_SACH;
        return;
    }
    if (!ChuyenChuoiThanhSoNguyen(CatKhoangTrang(currentState.chuoiSoTrang), soTrang, true) || soTrang <= 0 || soTrang > MAX_SO_TRANG) {
        std::string loiSoTrang = "Loi: So trang khong hop le!\nSo trang phai la so nguyen tu 1 den " + std::to_string(MAX_SO_TRANG) + ".\nVi du: 250, 500, 1000.";
        if (!currentState.chuoiSoTrang.empty()) {
            loiSoTrang += "\nGia tri nhap vao: '" + currentState.chuoiSoTrang + "'";
        }
        CapNhatThongBaoSFML(loiSoTrang, 1);
        inputHoatDong = INPUT_SO_TRANG;
        return;
    }
    loi = KiemTraChuoiVaDodai(currentState.chuoiTacGia, "Tac gia", MAX_TAC_GIA);
    if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        inputHoatDong = INPUT_TAC_GIA;
        return;
    }
    if (!ChuyenChuoiThanhSoNguyen(CatKhoangTrang(currentState.chuoiNamXB), namXB, true)) {
        std::string loiNam = "Loi: Nam xuat ban phai la so nguyen 4 chu so!\nVi du: 2020, 2021, 2024.\nGia tri nhap vao: '" + currentState.chuoiNamXB + "'";
        CapNhatThongBaoSFML(loiNam, 1);
        inputHoatDong = INPUT_NAM_XB;
        return;
    }
    loi = KiemTraNamXuatBan(namXB);
    if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        inputHoatDong = INPUT_NAM_XB;
        return;
    }
    loi = KiemTraChuoiVaDodai(currentState.chuoiTheLoai, "The loai", MAX_THE_LOAI);
    if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        inputHoatDong = INPUT_THE_LOAI;
        return;
    }

    if (!currentState.dangSua) {
        // Kiem tra loi rieng cho Them (ISBN, So Luong)
        loi = KiemTraChuoiRong(currentState.chuoiISBN, "ISBN");
        if (!loi.empty()) {
            CapNhatThongBaoSFML(loi, 1);
            inputHoatDong = INPUT_ISBN;
            return;
        }

        loi = ChuanHoaISBNFile(currentState.chuoiISBN, isbnChuan); // Chuan hoa + Kiem tra 10/13 so
        if (!loi.empty()) {
            CapNhatThongBaoSFML(loi, 1);
            inputHoatDong = INPUT_ISBN;
            return;
        }

        loi = KiemTraTrungISBN(isbnChuan); // Kiem tra trung lap
        if (!loi.empty()) {
            CapNhatThongBaoSFML(loi, 1);
            inputHoatDong = INPUT_ISBN;
            return;
        }

        if (!ChuyenChuoiThanhSoNguyen(CatKhoangTrang(currentState.chuoiSoLuong), soLuong, true) || soLuong <= 0 || soLuong > MAX_BAN_SAO) {
            std::string loiSoLuong = "Loi: So luong ban sao khong hop le!\nSo luong phai la so nguyen tu 1 den " + std::to_string(MAX_BAN_SAO) + ".\nVi du: 1, 5, 10 ban sao.";
            if (!currentState.chuoiSoLuong.empty()) {
                loiSoLuong += "\nGia tri nhap vao: '" + currentState.chuoiSoLuong + "'";
            }
            CapNhatThongBaoSFML(loiSoLuong, 1);
            inputHoatDong = INPUT_SO_LUONG;
            return;
        }

        loi = KiemTraTongSoBanSao(isbnChuan, soLuong); // Kiem tra tong so ban sao khong vuot qua MAX
        if (!loi.empty()) {
            CapNhatThongBaoSFML(loi, 1);
            inputHoatDong = INPUT_SO_LUONG;
            return;
        }

        // Goi backend them dau sach
        bool ok = themDauSach(dsDauSach, soLuongDauSach, isbnChuan, currentState.chuoiTenSach, soTrang, currentState.chuoiTacGia, namXB, currentState.chuoiTheLoai, true); // true = an lang (khong cout)
        if (!ok) {
            CapNhatThongBaoSFML("Loi: Khong them duoc (Backend error?)!", 1);
            return;
        }

        // Tim lai dau sach vua them de them ban sao
        PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbnChuan);
        if (dauSach) {
            int soThuTu = 1;
            int themThanhCong = 0;
            std::string loiTrongLoop = ""; // Bien luu loi neu co trong luc them ban sao

            // Vong lap them tung ban sao
            for (int i = 0; i < soLuong; ++i) {
                // Dung stream de bat loi tu ham backend (thay cho cout)
                std::stringstream errorStream;
                std::string maSach = sinhMaSach(isbnChuan, soThuTu); // Ham backend tra ve Ma Sach hoac "Loi:..." neu loi

                // Kiem tra loi tu sinhMaSach - SAI: check empty(), DUNG: check "Loi:"
                if (maSach.rfind("Loi:", 0) == 0) {
                    soThuTu++;                        // Thu sinh so tiep theo
                    loiTrongLoop = maSach;            // Lay chuoi loi
                    break; // Thoat khoi vong for neu sinh loi
                }

                // Goi backend them ban sao vao DSLK
                std::string loiThemDMS = themDanhMucSach(dauSach->dms, maSach, CHO_MUON_DUOC, "Ke Chinh"); // Ham backend tra ve "" hoac chuoi loi
                // Kiem tra loi tu themDanhMucSach
                if (!loiThemDMS.empty()) {
                    loiTrongLoop = loiThemDMS;
                    break; // Thoat khoi vong for neu them loi
                }

                // Neu khong co loi
                themThanhCong++;
                soThuTu++; // Tang so thu tu cho ban sao tiep theo
            }

            // Cap nhat tong ban sao tu backend (thay vi set thu cong)
            CapNhatTongBanSao(dsDauSach, soLuongDauSach);
            duLieuDaThayDoi = true;

            // Bao loi len UI neu co loi trong vong lap
            if (!loiTrongLoop.empty()) {
                std::string thongBao = "Them DS OK, chi them " + std::to_string(themThanhCong) + "/" + std::to_string(soLuong) + " BS!";
                CapNhatThongBaoSFML(thongBao, 1); // Loai 1 = Mau do
            }
            else {
                // Bao thanh cong len UI - cat ngan ten sach
                std::string tenHienThi = CatChuoiVoiDauCham(ChuyenThanhTitleCase(currentState.chuoiTenSach), 30);
                CapNhatThongBaoSFML("Them thanh cong: " + tenHienThi, 2);
                currentState.vuaThucHienThanhCong = true;                                // Dat co de khong bi de thong bao
            }

            // Reset form, tai lai bang
            XoaFormNhapLieuSFML(currentState);
            ThucHienTimKiemNoiBo(currentState);
            inputHoatDong = KHONG_XAC_DINH;
            // TOI UU: Cap nhat du lieu the loai neu dang o che do do
            if (currentState.cheDoXemHienTai == XEM_THEO_THE_LOAI) {
                CapNhatDuLieuXemTheoTheLoai(currentState);
            }
        }
        else {
            CapNhatThongBaoSFML("Loi: Khong tim thay DauSach vua them?!", 1);
        }
    }
    else { //
        // Goi backend cap nhat dau sach
        loi = CapNhatDauSach(dsDauSach, soLuongDauSach, currentState.isbnSachDuocChon, currentState.chuoiTenSach, soTrang, currentState.chuoiTacGia, namXB, currentState.chuoiTheLoai);

        // Bao ket qua len UI
        if (loi.empty()) {
            // cat ngan ten sach
            std::string tenHienThi = CatChuoiVoiDauCham(ChuyenThanhTitleCase(currentState.chuoiTenSach), 30);
            CapNhatThongBaoSFML("Cap nhat thanh cong: " + tenHienThi, 2);
            currentState.vuaThucHienThanhCong = true; // Dat co
            // Reset form, tai lai bang, bo chon sach
            XoaFormNhapLieuSFML(currentState);
            ThucHienTimKiemNoiBo(currentState);
            currentState.dangSua = false;
            inputHoatDong = KHONG_XAC_DINH;
            currentState.isbnSachDuocChon = "";
            // TOI UU: Cap nhat du lieu the loai neu dang o che do do
            if (currentState.cheDoXemHienTai == XEM_THEO_THE_LOAI) {
                CapNhatDuLieuXemTheoTheLoai(currentState);
            }
        }
        else {
            CapNhatThongBaoSFML(loi, 1);
        } // Bao loi neu co
    }
}

// Goi backend de Xoa Dau Sach
static void ThucHienXoaSachSFML(SachState& currentState) {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    extern bool duLieuDaThayDoi;
    (void)duLieuDaThayDoi; // Danh dau khong dung truc tiep

    if (currentState.isbnSachDuocChon.empty()) {
        CapNhatThongBaoSFML("Loi: Chua chon dau sach de xoa!", 1);
        return;
    }

    // Goi backend xoa
    std::string loi = XoaDauSachTheoISBN(dsDauSach, soLuongDauSach, currentState.isbnSachDuocChon); // Ham tra ve "" hoac chuoi loi

    // Bao ket qua len UI
    if (loi.empty()) {
        CapNhatThongBaoSFML("Xoa dau sach thanh cong!", 2);
        currentState.isbnSachDuocChon = "";  // Bo chon sach da xoa
        ThucHienTimKiemNoiBo(currentState); // Tai lai bang
        // TOI UU: Cap nhat du lieu the loai neu dang o che do do
        if (currentState.cheDoXemHienTai == XEM_THEO_THE_LOAI) {
            CapNhatDuLieuXemTheoTheLoai(currentState);
        }
    }
    else {
        CapNhatThongBaoSFML(loi, 1);
    } // Bao loi neu co
    currentState.xacNhanXoa = false; // An hop thoai xac nhan
}
