#include "include/ManHinhQuanLySach.h"
#include "include/GiaoDienSFML.h"
#include "include/TienIchGiaoDien.h"
#include "include/QuanLySach.h"
#include "include/KiemTraDuLieu.h"
#include "include/XuLyChuoi.h"
#include "include/Constants.h"
#include "include/NgayThang.h"
#include <iostream>
#include <algorithm>
#include <SFML/System/Clock.hpp>
#include <sstream>




// Bien trang thai CUC BO cho Man Hinh Sach
static KetQuaTimKiem ketQuaTimKiem[MAX_DAUSACH];
static int soLuongKetQuaTimKiem = 0;
static int trangHienTai = 1;
static int tongSoTrang = 1;

// trang thai che do xem
enum CheDoXemSach {
    XEM_TIM_KIEM,     // che do mac dinh: bang phan trang, tim kiem
    XEM_THEO_THE_LOAI // che do xem gom nhom theo the loai
};
static CheDoXemSach cheDoXemHienTai = XEM_TIM_KIEM; // mac dinh la xem tim kiem
static float scrollOffsetYTheLoai = 0.f;            // Do lech Y khi cuon
static float totalContentHeightTheLoai = 0.f;       // Tong chieu cao noi dung
static sf::View theLoaiView;
static sf::RenderTexture theLoaiRenderTexture;

static std::string chuoiTimKiem = "";
static std::string chuoiISBN = "";
static std::string chuoiTenSach = "";
static std::string chuoiSoTrang = "";
static std::string chuoiTacGia = "";
static std::string chuoiNamXB = "";
static std::string chuoiTheLoai = "";
static std::string chuoiSoLuong = "";

static std::string isbnSachDuocChon = "";
static bool dangSua = false;
static bool xacNhanXoa = false;
static bool vuaThucHienThanhCong = false;

static sf::Clock doubleClickClock;
static std::string isbnClickCuoi = "";
static const float THOI_GIAN_DOUBLE_CLICK = 0.3f;
static std::string isbnRowClicked = "";

// Bien cho Modal Chi Tiet
static bool hienThiModalBanSao = false;
static std::string idModalSachDuocChon = "";
static int trangModal = 1;
const int SO_SACH_MOI_TRANG_MODAL = 10;

struct NutModal {
    std::string maSach;
    sf::FloatRect khuVucNhanThanhLy;
    sf::FloatRect khuVucNhanXoa;
};

// mang luu nut dong
static NutModal cacNutThanhLyTrongModal[SO_SACH_MOI_TRANG_MODAL];
static int soLuongNutModal = 0;

// biben cho Xac Nhan Thanh Ly
static bool xacNhanThanhLy = false;
static std::string maSachCanThanhLy = "";

// Bien cho Xac Nhan Xoa Ban Sao
static bool xacNhanXoaBanSao = false;
static std::string maSachCanXoa = "";

// Bien cho Modal Them Ban Sao
static bool hienThiModalThemBS = false;
static std::string soLuongBanSaoCanThemStr = "";
static std::string viTriBanSaoCanThemStr = ""; // Them bien vi tri



// khai bao ham noi bo (static)
static void VeBangSach(sf::RenderWindow &window, const sf::Font &font);
static void VeFormThemSach(sf::RenderWindow &window, const sf::Font &font);
static void VeKhungThongBaoSFML(sf::RenderWindow &window, const sf::Font &font);
static void XuLyTextInput(sf::Event event);
static void ThucHienTimKiemNoiBo();
static void ThucHienThemHoacSuaSachSFML();
static void ThucHienXoaSachSFML();
static void XoaFormNhapLieuSFML();
static void DienFormVoiSachDuocChon();
static void CapNhatPhanTrangSFML();
// Ham cho Modal
static void ThucHienThanhLySach(std::string maSach);
static void VeModalChiTietBanSao(sf::RenderWindow &window, const sf::Font &font);



//DINH NGHIA STACK UNDO
const int MAX_UNDO_STEPS = 20; // Luu toi da 20 buoc hoan tac

// Node luu 1 trang thai van ban
struct UndoStackNode {
    std::string data;
};

// Cau truc Stack
struct UndoStack {
    UndoStackNode history[MAX_UNDO_STEPS]; // Mang luu lich su
    int top;                               // Chi so dinh stack (-1 la rong)
};

// Khai bao stack undo cuc bo cho file nay
static UndoStack undoStack;
static bool stackDaKhoiTao = false; // Co de khoi tao stack 1 lan

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

// Ham Ve

void VeBangSach(sf::RenderWindow &window, const sf::Font &font){
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
    int startIndex = (trangHienTai - 1) * SACH_MOI_TRANG;
    int endIndex = (startIndex + SACH_MOI_TRANG < soLuongKetQuaTimKiem)
                       ? (startIndex + SACH_MOI_TRANG)
                       : soLuongKetQuaTimKiem;

    float rowHeight = 30.f;

    sf::Text dataText;
    dataText.setFont(font);
    dataText.setCharacterSize(FONT_SIZE_BINH_THUONG);

    for (int i = startIndex; i < endIndex; ++i) {
        PTRDS d = ketQuaTimKiem[i].sach;
        if (!d)
            continue;

        bool isSelected = (d->ISBN == isbnSachDuocChon);
        if (isSelected) {
            sf::RectangleShape highlight(sf::Vector2f(BANG_RONG - PADDING * 0.8, rowHeight));
            highlight.setPosition(BANG_X + PADDING * 0.4, currentY - 2.f);
            highlight.setFillColor(MAU_NHAN);
            window.draw(highlight);
        }
        dataText.setFillColor(isSelected ? sf::Color::Black : MAU_CHU);

        // Cat chuoi neu qua dai
        std::string tenSachSub = d->tenSach.length() > 25 ? d->tenSach.substr(0, 22) + "..." : d->tenSach;
        std::string tacGiaSub = d->tacGia.length() > 15 ? d->tacGia.substr(0, 12) + "..." : d->tacGia;
        std::string theLoaiSub = d->theLoai.length() > 8 ? d->theLoai.substr(0, 5) + "..." : d->theLoai;

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

    if (!isbnSachDuocChon.empty()) {
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

    sf::Text pageInfo = TaoVanBan(font, "Trang " + std::to_string(trangHienTai) + " / " + std::to_string(tongSoTrang), 16, MAU_CHU);
    pageInfo.setPosition(BANG_X + 260.f, pageY + NUT_CAO / 2.f - pageInfo.getLocalBounds().height / 2.f - 2.f);
    window.draw(pageInfo);
}

void VeFormThemSach(sf::RenderWindow &window, const sf::Font &font) {
    // Lay constants
    static int namHienTai = LayNamHienTai();
    std::string goiYNamXB = std::to_string(NAM_XUAT_BAN_MIN) + "-" + std::to_string(namHienTai);
    std::string goiYSoTrang = "1 - " + std::to_string(MAX_SO_TRANG);
    std::string goiYSoLuong = "1 - " + std::to_string(MAX_BAN_SAO) + " ban sao";
    std::string goiYISBN = "10 hoac 13 chu so";
    std::string goiYTenSach = "Toi da " + std::to_string(MAX_TEN_SACH) + " ky tu";
    std::string goiYTacGia = "Toi da " + std::to_string(MAX_TAC_GIA) + " ky tu";
    std::string goiYTheLoai = "Toi da " + std::to_string(MAX_THE_LOAI) + " ky tu";

    std::string formTieuDe = dangSua ? "HIEU CHINH DAU SACH" : "THEM DAU SACH";

    // Tinh chieu cao form cho khop
    float formHeight = KHUNG_THONG_BAO_Y - FORM_Y - PADDING;
    VeKhung(window, FORM_X, FORM_Y, FORM_RONG, formHeight, formTieuDe, font);
    float currentY = FORM_Y + 40.f;
    float labelX = FORM_X + PADDING;
    float inputSpacing = PADDING / 2.f;

    TaoInput(font, INPUT_ISBN, labelX, currentY, INPUT_RONG, INPUT_CAO, "ISBN (*):", chuoiISBN, goiYISBN, dangSua);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_TEN_SACH, labelX, currentY, INPUT_RONG, INPUT_CAO, "Ten Sach (*):", chuoiTenSach, goiYTenSach);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_SO_TRANG, labelX, currentY, INPUT_RONG, INPUT_CAO, "So Trang (*):", chuoiSoTrang, goiYSoTrang);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_TAC_GIA, labelX, currentY, INPUT_RONG, INPUT_CAO, "Tac Gia (*):", chuoiTacGia, goiYTacGia);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_NAM_XB, labelX, currentY, INPUT_RONG, INPUT_CAO, "Nam XB (*):", chuoiNamXB, goiYNamXB);
    currentY += INPUT_CAO + inputSpacing;
    TaoInput(font, INPUT_THE_LOAI, labelX, currentY, INPUT_RONG, INPUT_CAO, "The Loai (*):", chuoiTheLoai, goiYTheLoai);
    currentY += INPUT_CAO + inputSpacing;

    if (!dangSua) {
        TaoInput(font, INPUT_SO_LUONG, labelX, currentY, INPUT_RONG, INPUT_CAO, "So Luong (*):", chuoiSoLuong, goiYSoLuong);
        currentY += INPUT_CAO + PADDING * 1.5f;
    }
    else {
        currentY += PADDING * 1.5f;
    }

    float totalWidth = FORM_RONG - 2 * PADDING;
    float nutRong = (totalWidth - PADDING) / 2;
    float nutCao = NUT_CAO + 10.f;

    std::string nutChinhLabel = dangSua ? "LUU THAY DOI" : "THEM SACH";

    if (dangSua) {
        TaoNut(font, NUT_THEM_SACH, labelX, currentY, nutRong, nutCao, nutChinhLabel, MAU_NHAN, MAU_CHU_NUT);
        TaoNut(font, NUT_BACK, labelX + nutRong + PADDING, currentY, nutRong, nutCao, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
    }
    else {
        float nutRongDon = 250.f;
        float nutXDon = labelX + (totalWidth - nutRongDon) / 2;
        TaoNut(font, NUT_THEM_SACH, nutXDon, currentY, nutRongDon, nutCao, nutChinhLabel, MAU_NHAN, MAU_CHU_NUT);
    }
}

void VeKhungThongBaoSFML(sf::RenderWindow &window, const sf::Font &font) {
    VeKhung(window, FORM_X, KHUNG_THONG_BAO_Y, FORM_RONG, KHUNG_THONG_BAO_CAO, "THONG BAO", font);

    if (xacNhanXoa) { // Ve hop thoai xac nhan xoa
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
        // Ve thong bao binh thuong
        sf::Color mauChuThongBao = MAU_CHU;
        if (loaiThongBao == 1)
            mauChuThongBao = MAU_LOI;
        else if (loaiThongBao == 2)
            mauChuThongBao = MAU_THANH_CONG;
        sf::Text txtMsg = TaoVanBan(font, noiDungThongBao, FONT_SIZE_BINH_THUONG, mauChuThongBao);

        // Can giua thong bao
        txtMsg.setPosition(FORM_X + PADDING, KHUNG_THONG_BAO_Y + (KHUNG_THONG_BAO_CAO - txtMsg.getCharacterSize()) / 2.f - 10.f);
        window.draw(txtMsg);
    }
}

// Ham logic goi backend de them ban sao
static void ThucHienThemBanSao() {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    extern bool duLieuDaThayDoi;

    int soLuongThem = 0;
    // Kiem tra chuoi so luong nhap vao
    if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(soLuongBanSaoCanThemStr), soLuongThem, true) || soLuongThem <= 0) {
        CapNhatThongBaoSFML("Loi: So luong them phai la so nguyen duong!", 1);
        inputHoatDong = INPUT_SO_LUONG_THEM; // Focus lai vao o input
        return;
    }

    // Tim Dau Sach can them
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbnSachDuocChon);
    if (!dauSach) {
        CapNhatThongBaoSFML("Loi: Khong tim thay dau sach de them ban sao!", 1);
        hienThiModalThemBS = false; // Dong modal
        return;
    }

    // Kiem tra tong so ban sao khong vuot qua MAX
    std::string loiTong = KiemTraTongSoBanSao(isbnSachDuocChon, soLuongThem);
    if (!loiTong.empty()) {
        CapNhatThongBaoSFML(loiTong, 1);
        inputHoatDong = INPUT_SO_LUONG_THEM;
        return;
    }

    // Lay vi tri tu input hoac tu ban sao cu
    std::string viTriDeThem = CatKhoangTrang(viTriBanSaoCanThemStr);
    
    // Neu khong nhap vi tri, lay tu ban sao cu (neu co)
    if (viTriDeThem.empty()) {
        if (dauSach->dms != nullptr) {
            viTriDeThem = dauSach->dms->viTri;
        } else {
            viTriDeThem = "Ke Chinh"; // Mac dinh neu chua co ban sao nao
        }
    }
    
    // Kiem tra do dai vi tri
    if (viTriDeThem.length() > MAX_VI_TRI_KE) {
        CapNhatThongBaoSFML("Loi: Vi tri qua dai (toi da " + std::to_string(MAX_VI_TRI_KE) + " ky tu)!", 1);
        inputHoatDong = INPUT_VI_TRI_THEM;
        return;
    }

    // Backend sinhMaSach() tu check trung, khong can tim maxIndex
    int soThuTu = 1; // Bat dau tu 1, backend se tu tang neu trung

    // Vong lap them ban sao
    int themThanhCong = 0;
    std::string loiTrongLoop = "";
    for (int i = 0; i < soLuongThem; ++i) {

        std::string maSachResult = sinhMaSach(isbnSachDuocChon, soThuTu);
        // Kiem tra xem chuoi tra ve co bat dau bang "Loi:" khong
        // rfind tra ve 0 neu tim thay chuoi con o vi tri bat dau (0)
        if (maSachResult.rfind("Loi:", 0) == 0) {
            loiTrongLoop = maSachResult; // Gan chuoi loi tra ve
            soThuTu++; // Van phai tang soThuTu de lan sau (neu co) thu so khac
            break; // Thoat vong lap for neu sinh loi
        }
        // Neu khong co loi:
        std::string maSach = maSachResult; // Lay ma sach thanh cong

        // Goi backend them ban sao vao DSLK (dung viTriDeThem)
        std::string loiThemDMS = themDanhMucSach(dauSach->dms, maSach, CHO_MUON_DUOC, viTriDeThem);
        // Kiem tra loi tu themDanhMucSach
        if (!loiThemDMS.empty()) {
            loiTrongLoop = loiThemDMS;
            break; // Thoat vong lap for neu them loi
        }

        // Neu khong co loi
        themThanhCong++;
        soThuTu++; // Tang so thu tu cho ban sao tiep theo
    }

    // Cap nhat tong so ban sao tu backend (thay vi cong thu cong)
    CapNhatTongBanSao(dsDauSach, soLuongDauSach);
    duLieuDaThayDoi = true; // Danh dau de luu file

    // Bao loi len UI neu co loi trong vong lap
    if (!loiTrongLoop.empty()) {
        std::string thongBao = "Chi them duoc " + std::to_string(themThanhCong) + "/" + std::to_string(soLuongThem) + " ban sao. Loi: " + loiTrongLoop;
        CapNhatThongBaoSFML(thongBao, 1); // Loai 1 = Mau do
    } else { // Neu khong co loi
        // Bao thanh cong len UI
        CapNhatThongBaoSFML("Them thanh cong " + std::to_string(themThanhCong) + " ban sao cho: " + dauSach->tenSach, 2); // Loai 2 = Mau xanh
        vuaThucHienThanhCong = true; // Dat co de khong bi de thong bao Tim Kiem
    }

    hienThiModalThemBS = false;     // Dong modal
    soLuongBanSaoCanThemStr = "";   // Xoa input so luong
    viTriBanSaoCanThemStr = "";     // Xoa input vi tri
    inputHoatDong = KHONG_XAC_DINH; // Deactive input
    ThucHienTimKiemNoiBo();         // Tai lai bang chinh de cap nhat cot SL
}

// Ham ve modal Them Ban Sao
static void VeModalThemBanSao(sf::RenderWindow &window, const sf::Font &font) {
    // Ve lop nen mo
    sf::RectangleShape overlay(sf::Vector2f(CHIEU_RONG, CHIEU_CAO));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // Dinh nghia kich thuoc va vi tri modal
    float modalRong = 500.f; // Tang rong 1 chut
    float modalCao = 260.f;  // Tang cao de chua 2 input
    float modalX = (CHIEU_RONG - modalRong) / 2.f;
    float modalY = (CHIEU_CAO - modalCao) / 2.f;
    float paddingNoiBo = PADDING;

    // Ve khung modal
    VeKhung(window, modalX, modalY, modalRong, modalCao, "THEM BAN SAO", font);

    // Ve o Input so luong
    float inputY = modalY + 60.f;
    float inputHopRong = modalRong - 2 * paddingNoiBo;
    TaoInput(font, INPUT_SO_LUONG_THEM, modalX + paddingNoiBo - 110.f, inputY, inputHopRong, INPUT_CAO, "", soLuongBanSaoCanThemStr, "So luong (1-" + std::to_string(MAX_BAN_SAO) + ")");
    
    // Ve o Input vi tri (ben duoi so luong)
    inputY += INPUT_CAO + PADDING / 2.f;
    TaoInput(font, INPUT_VI_TRI_THEM, modalX + paddingNoiBo - 110.f, inputY, inputHopRong, INPUT_CAO, "", viTriBanSaoCanThemStr, "Vi tri (VD: Ke Chinh, Ke A1, Tu 5...)");

    // Ve nut Xac Nhan / Huy
    float buttonY = modalY + modalCao - NUT_CAO - paddingNoiBo;
    float totalWidth = modalRong - 2 * paddingNoiBo;
    float nutRong = (totalWidth - paddingNoiBo) / 2.f;
    TaoNut(font, NUT_XAC_NHAN_THEM_BS, modalX + paddingNoiBo, buttonY, nutRong, NUT_CAO, "XAC NHAN", MAU_NHAN, MAU_CHU_NUT);
    TaoNut(font, NUT_HUY_THEM_BS, modalX + paddingNoiBo + nutRong + paddingNoiBo, buttonY, nutRong, NUT_CAO, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
}

static void VeDanhSachTheoTheLoai(sf::RenderWindow& window, const sf::Font& font) {
    // buoc 0: dinh nghia vung ve noi dung
    float headerY = BANG_Y + 50.f;                  // y bat dau cua khung (so voi cua so)
    float contentY = headerY + 40.f;                 // y bat dau cua NOI DUNG ben trong khung (so voi cua so)
    float contentBottom = CHIEU_CAO - (PADDING * 3.0f) - (NUT_CAO * 2); // dung 3.0f (day khung)
    float contentWidth = BANG_RONG;                  // chieu rong khung = chieu rong vung noi dung
    float contentHeight = contentBottom - contentY;    // chieu cao THUC TE cua vung noi dung
    contentHeight = std::max(contentHeight, 10.f); // dam bao khong am

    // buoc 1: ve khung chua danh sach (ve truoc)
    // ve khung voi chieu cao tong cong (bao gom ca header cua khung)
    VeKhung(window, BANG_X, headerY, contentWidth, contentHeight + (contentY - headerY), "DANH SACH DAU SACH THEO THE LOAI (Cuon chuot de xem)", font);

    // lay du lieu tu backend
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;

    // buoc 2: lay danh sach cac the loai duy nhat va sap xep
    std::string cacTheLoai[MAX_DAUSACH]; // mang tam luu ten the loai
    int soTheLoai = TimTheLoaiDuyNhat(dsDauSach, soLuongDauSach, cacTheLoai);
    SapXepTheLoaiTheoTen(cacTheLoai, soTheLoai);

    // buoc 3: tinh toan tong chieu cao noi dung (de biet co can scroll khong)
    totalContentHeightTheLoai = PADDING; // bat dau voi padding top
    float lineSpacing = 8.f;             // khoang cach giua 2 dong chu cua 1 sach
    float groupSpacing = 20.f;           // khoang cach giua cac nhom the loai (gom duong ke)
    float afterTitleSpacing = 35.f;      // khoang cach sau tieu de the loai
    float afterGroupSpacing = 10.f;      // khoang cach duoi cung cua nhom sach
    float textBlockHeight = FONT_SIZE_BINH_THUONG * 2 + lineSpacing; // chieu cao uoc tinh cua 1 muc sach (2 dong)

    // vong lap nay cong don chieu cao cua tung phan tu
    for (int i = 0; i < soTheLoai; ++i) {
        if (i > 0) totalContentHeightTheLoai += groupSpacing;
        totalContentHeightTheLoai += afterTitleSpacing;
        PTRDS tempSach[MAX_DAUSACH]; // mang tam de dem so luong sach
        int soSach = TimSachTheoTheLoai(dsDauSach, soLuongDauSach, cacTheLoai[i], tempSach); // lay so luong sach
        totalContentHeightTheLoai += (float)soSach * textBlockHeight; // cong them tong chieu cao cua cac sach trong nhom
        totalContentHeightTheLoai += afterGroupSpacing; // cong them khoang cach duoi nhom
    }
    totalContentHeightTheLoai += PADDING; // cong them padding duoi cung
    totalContentHeightTheLoai = std::max(totalContentHeightTheLoai, contentHeight); // dam bao khong nho hon vung nhin

    // buoc 3: gioi han scrollOffsetYTheLoai (vi tri cuon)
    float maxScroll = 0.f; // vi tri cuon toi da
    if (totalContentHeightTheLoai > contentHeight) {
        maxScroll = totalContentHeightTheLoai - contentHeight; // tong chieu cao - chieu cao nhin thay
    }
    if (scrollOffsetYTheLoai < 0.f) scrollOffsetYTheLoai = 0.f; // chan cuon len qua dinh
    if (scrollOffsetYTheLoai > maxScroll) scrollOffsetYTheLoai = maxScroll; // chan cuon xuong qua day


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
    theLoaiView.setViewport(sf::FloatRect(
        BANG_X / (float)CHIEU_RONG,        // left (giong X cua khung)
        viewportTop,                       // top (da tinh o tren)
        rongNoiDungView / (float)CHIEU_RONG, // width (da tru cho thanh cuon)
        viewportHeight                     // height (da tinh o tren)
    ));
    
    // thiet lap "the gioi" ben trong camera
    theLoaiView.setSize(rongNoiDungView, contentHeight); // kich thuoc "the gioi"
    // dich chuyen camera theo vi tri cuon
    theLoaiView.setCenter(rongNoiDungView / 2.f, contentHeight / 2.f + scrollOffsetYTheLoai);

    // buoc 5: Ap dung View (bat camera)
    window.setView(theLoaiView);

    // ham ho tro cat chuoi theo pixel (dinh nghia noi bo)
    auto CatChuoiTheoRong = [&](const std::string& src, unsigned int coChu, float rongToiDa) {
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
    };

    // buoc 6: Ve noi dung ben trong View (toa do tuong doi voi View)
    // view se tu dong cat moi thu ve ra ngoai khu vuc cua no
    float currentY = PADDING; // y bat dau tuong doi (so voi view)
    float contentX = PADDING; // x bat dau tuong doi (so voi view)
    float bookIndentX = contentX + 35.f; // thut le cho sach
    float lineWidth = rongNoiDungView - 2 * PADDING; // chieu rong duong ke (theo view)

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

            // cat chuoi cho vua
            std::string tenSachSub = CatChuoiTheoRong(sach->tenSach, FONT_SIZE_BINH_THUONG, rongChoNoiDung);
            std::string metaInfo = "   ISBN: " + sach->ISBN + " | TG: " + sach->tacGia + " | NXB: " + std::to_string(sach->namXuatBan);
            std::string metaInfoSub = CatChuoiTheoRong(metaInfo, FONT_SIZE_BINH_THUONG, rongChoNoiDung);
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
    if (totalContentHeightTheLoai > contentHeight) {
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
        float handleHeight = scrollBarHeight * (contentHeight / totalContentHeightTheLoai);
        handleHeight = std::max(handleHeight, 20.f); // chieu cao toi thieu
        float handleY = scrollBarY;
        if (maxScroll > 0) { // tinh vi tri y cua tay cam
             handleY = scrollBarY + (scrollOffsetYTheLoai / maxScroll) * (scrollBarHeight - handleHeight);
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
    TaoNut(font, NUT_BACK, CHIEU_RONG - PADDING - 100.f, PADDING / 4, 100.f, NUT_CAO * 0.8f, "< MENU", MAU_NUT_BACK, MAU_CHU_NUT);
    // ve o tim kiem va cac nut lien quan
    TaoInput(font, INPUT_TIM_SACH, BANG_X, BANG_Y, 500.f, INPUT_CAO, "Tim Kiem:", chuoiTimKiem);
    TaoNut(font, NUT_TIM, BANG_X + 110.f + 500.f + 10.f, BANG_Y, 100.f, NUT_CAO, "TIM", MAU_NHAN, MAU_CHU_NUT);
    TaoNut(font, NUT_XOA_TIM, BANG_X + 110.f + 500.f + 10.f + 100.f + 10.f, BANG_Y, 100.f, NUT_CAO, "XOA TIM", MAU_NEN_NUT, MAU_CHU_NUT);

    float nutChuyenX = BANG_X + 110.f + 500.f + 10.f + 100.f + 10.f + 100.f + PADDING; // sau nut Xoa Tim
    // thay doi nhan nut tuy theo che do hien tai
    std::string nhanNutChuyen = (cheDoXemHienTai == XEM_TIM_KIEM) ? "Xem theo The Loai" : "Xem Tim Kiem/Tat Ca";
    TaoNut(font, NUT_CHUYEN_CHE_DO_XEM, nutChuyenX, BANG_Y, 220.f /* rong hon chut */, NUT_CAO, nhanNutChuyen, MAU_NEN_NUT, MAU_CHU_NUT);

    // Goi ham ve bang/danh sach tuy theo che do ---
    if (cheDoXemHienTai == XEM_TIM_KIEM) {
        VeBangSach(window, font); // ve bang phan trang nhu cu neu la che do tim kiem
    }
    else {
        VeDanhSachTheoTheLoai(window, font); // ve danh sach gom nhom neu la che do the loai
    }

    VeFormThemSach(window, font);      // ve form them/sua sach ben phai
    VeKhungThongBaoSFML(window, font); // ve khung thong bao ben duoi form

    if (hienThiModalBanSao) { // modal chi tiet ban sao
        VeModalChiTietBanSao(window, font);
    }
    else if (hienThiModalThemBS) { // modal them ban sao
        VeModalThemBanSao(window, font);
    }
}

// Ham xu ly backend cho modal
static void ThucHienThanhLySach(std::string maSach) {
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
static void ThucHienXoaBanSao(std::string maSach) {
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
static void VeModalChiTietBanSao(sf::RenderWindow &window, const sf::Font &font) {
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
    PTRDS dauSach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, idModalSachDuocChon);
    if (!dauSach) {
        CapNhatThongBaoSFML("Loi: Khong tim thay dau sach de xem chi tiet!", 1);
        hienThiModalBanSao = false; // Dong modal neu loi
        return;
    }

    // Ve khung modal va nut Dong
    VeKhung(window, modalX, modalY, modalRong, modalCao, "CHI TIET BAN SAO: " + dauSach->tenSach, font);
    
    // Them ghi chu huong dan cho user (de hieu workflow)
    sf::Text hintText = TaoVanBan(font, "Luu y: 'Thanh Ly' truoc, sau do 'Xoa' de loai bo khoi he thong", FONT_SIZE_NHO - 2, sf::Color(150, 150, 150));
    hintText.setPosition(modalX + paddingNoiBo, modalY + 35.f);
    window.draw(hintText);
    
    // Hien thi so luong ban sao
    int tongBanSao = 0;
    PTRDMS pCount = dauSach->dms;
    while (pCount) { tongBanSao++; pCount = pCount->next; }
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
    int sttBatDau = (trangModal - 1) * SO_SACH_MOI_TRANG_MODAL;
    int sttKetThuc = sttBatDau + SO_SACH_MOI_TRANG_MODAL;
    sttKetThuc = std::min(sttKetThuc, soLuongBSTrongDSLK); // Khong vuot qua so luong thuc te

    soLuongNutModal = 0; // Reset mang nut dong

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
            if (soLuongNutModal < SO_SACH_MOI_TRANG_MODAL) {
                cacNutThanhLyTrongModal[soLuongNutModal].maSach = p->maSach;
                cacNutThanhLyTrongModal[soLuongNutModal].khuVucNhanThanhLy = nutTL.getGlobalBounds();
                cacNutThanhLyTrongModal[soLuongNutModal].khuVucNhanXoa = sf::FloatRect(); // Rong
                soLuongNutModal++;
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
            if (soLuongNutModal < SO_SACH_MOI_TRANG_MODAL) {
                cacNutThanhLyTrongModal[soLuongNutModal].maSach = p->maSach;
                cacNutThanhLyTrongModal[soLuongNutModal].khuVucNhanThanhLy = sf::FloatRect(); // Rong
                cacNutThanhLyTrongModal[soLuongNutModal].khuVucNhanXoa = nutXoa.getGlobalBounds();
                soLuongNutModal++;
            }
        }
        // DANG_MUON -> Khong co nut nao (hien dau '-')
        
        currentY += 30.f; // Xuong hang
    } // Ket thuc vong for

    // Ve Hop Thoai Xac Nhan Thanh Ly
    if (xacNhanThanhLy) {
        sf::RectangleShape confirmOverlay(sf::Vector2f(modalRong, modalCao));
        confirmOverlay.setPosition(modalX, modalY);
        confirmOverlay.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(confirmOverlay);
        float confirmRong = 400.f;
        float confirmCao = 150.f;
        float confirmX = modalX + (modalRong - confirmRong) / 2.f;
        float confirmY = modalY + (modalCao - confirmCao) / 2.f;
        VeKhung(window, confirmX, confirmY, confirmRong, confirmCao, "XAC NHAN THANH LY", font);
        sf::Text confirmText = TaoVanBan(font, "Ban co chac muon thanh ly ma sach:\n" + maSachCanThanhLy + "?", FONT_SIZE_BINH_THUONG, MAU_LOI);
        confirmText.setPosition(confirmX + paddingNoiBo, confirmY + 45.f);
        window.draw(confirmText);
        float confirmBtnY = confirmY + confirmCao - 50.f;
        float confirmBtnWidth = (confirmRong - 3 * paddingNoiBo) / 2.f;
        TaoNut(font, NUT_XAC_NHAN_THANHLY, confirmX + paddingNoiBo, confirmBtnY, confirmBtnWidth, 40.f, "XAC NHAN", MAU_LOI, MAU_CHU_NUT);
        TaoNut(font, NUT_HUY_THANHLY, confirmX + 2 * paddingNoiBo + confirmBtnWidth, confirmBtnY, confirmBtnWidth, 40.f, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
    }
    
    // Ve Hop Thoai Xac Nhan Xoa Ban Sao
    if (xacNhanXoaBanSao) {
        sf::RectangleShape confirmOverlay(sf::Vector2f(modalRong, modalCao));
        confirmOverlay.setPosition(modalX, modalY);
        confirmOverlay.setFillColor(sf::Color(0, 0, 0, 100));
        window.draw(confirmOverlay);
        float confirmRong = 400.f;
        float confirmCao = 150.f;
        float confirmX = modalX + (modalRong - confirmRong) / 2.f;
        float confirmY = modalY + (modalCao - confirmCao) / 2.f;
        VeKhung(window, confirmX, confirmY, confirmRong, confirmCao, "XAC NHAN XOA BAN SAO", font);
        sf::Text confirmText = TaoVanBan(font, "Ban co chac muon xoa ban sao:\n" + maSachCanXoa + "?", FONT_SIZE_BINH_THUONG, MAU_LOI);
        confirmText.setPosition(confirmX + paddingNoiBo, confirmY + 45.f);
        window.draw(confirmText);
        float confirmBtnY = confirmY + confirmCao - 50.f;
        float confirmBtnWidth = (confirmRong - 3 * paddingNoiBo) / 2.f;
        TaoNut(font, NUT_XAC_NHAN_XOA_BANSAO, confirmX + paddingNoiBo, confirmBtnY, confirmBtnWidth, 40.f, "XAC NHAN", MAU_LOI, MAU_CHU_NUT);
        TaoNut(font, NUT_HUY_XOA_BANSAO, confirmX + 2 * paddingNoiBo + confirmBtnWidth, confirmBtnY, confirmBtnWidth, 40.f, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
    }
    
    // Ve cac nut dieu khien phia duoi modal (Dong + Phan trang)
    float bottomY = modalY + modalCao - 50.f;
    
    // Tinh tong so trang modal
    int tongBanSaoModal = 0;
    PTRDMS pCountModal = dauSach->dms;
    while (pCountModal) { tongBanSaoModal++; pCountModal = pCountModal->next; }
    int tongSoTrangModal = (tongBanSaoModal + SO_SACH_MOI_TRANG_MODAL - 1) / SO_SACH_MOI_TRANG_MODAL;
    if (tongSoTrangModal < 1) tongSoTrangModal = 1;
    
    // Nut phan trang (neu co nhieu hon 1 trang)
    if (tongSoTrangModal > 1) {
        TaoNut(font, NUT_MODAL_TRANG_TRUOC, modalX + paddingNoiBo, bottomY, 80.f, 40.f, "<< Truoc", MAU_NEN_NUT, MAU_CHU_NUT);
        TaoNut(font, NUT_MODAL_TRANG_SAU, modalX + paddingNoiBo + 90.f, bottomY, 80.f, 40.f, "Sau >>", MAU_NEN_NUT, MAU_CHU_NUT);
        
        // Hien thi "Trang X / Y"
        sf::Text pageInfoModal = TaoVanBan(font, "Trang " + std::to_string(trangModal) + " / " + std::to_string(tongSoTrangModal), FONT_SIZE_NHO, MAU_CHU);
        pageInfoModal.setPosition(modalX + paddingNoiBo + 180.f, bottomY + 10.f);
        window.draw(pageInfoModal);
    }
    
    // Nut Dong (ben phai)
    TaoNut(font, NUT_DONG_MODAL_CHITIET, modalX + modalRong - 120.f - paddingNoiBo, bottomY, 120.f, 40.f, "DONG", MAU_LOI, MAU_CHU_NUT);
}

// Ham Xu Ly Su Kien CHINH
void XuLySuKienManHinhSach(sf::RenderWindow &window, sf::Event event) {
    (void)window;

    // Xu ly cuon chuot cho che do xem The Loai
    if (cheDoXemHienTai == XEM_THEO_THE_LOAI && !hienThiModalBanSao && !hienThiModalThemBS) { // Chi cuon khi o che do TL va khong co modal
        if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                // Dieu chinh scrollOffsetYTheLoai dua tren delta, 30.f la toc do cuon
                scrollOffsetYTheLoai -= event.mouseWheelScroll.delta * 30.f;

                // Gioi han lai scroll sau khi cuon
                float maxScroll = 0.f;
                if (totalContentHeightTheLoai > (CHIEU_CAO - (PADDING * 5.5f) - (NUT_CAO * 2) - (BANG_Y + 90.f))) { // Recalculate contentHeight equivalent
                    maxScroll = totalContentHeightTheLoai - (CHIEU_CAO - (PADDING * 5.5f) - (NUT_CAO * 2) - (BANG_Y + 90.f));
                }
                if (scrollOffsetYTheLoai < 0.f)
                    scrollOffsetYTheLoai = 0.f;
                if (scrollOffsetYTheLoai > maxScroll)
                    scrollOffsetYTheLoai = maxScroll;

                return; // Da xu ly scroll, khong can lam gi them voi su kien nay
            }
        }
    }

    // chan tat ca su kien neu modal dang hien
    if (hienThiModalBanSao || hienThiModalThemBS) {
        if (hienThiModalThemBS) { // Xu ly rieng cho Modal Them Ban Sao
            if (event.type == sf::Event::TextEntered || (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Backspace || event.key.code == sf::Keyboard::Tab))) {
                // Xu ly input So Luong
                if (inputHoatDong == INPUT_SO_LUONG_THEM) {
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode >= '0' && event.text.unicode <= '9' && soLuongBanSaoCanThemStr.length() < 3) {
                            soLuongBanSaoCanThemStr += static_cast<char>(event.text.unicode);
                            if (loaiThongBao == 1) CapNhatThongBaoSFML("", 0);
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Backspace) {
                        if (!soLuongBanSaoCanThemStr.empty()) {
                            soLuongBanSaoCanThemStr.pop_back();
                            if (loaiThongBao == 1) CapNhatThongBaoSFML("", 0);
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Tab) {
                        inputHoatDong = INPUT_VI_TRI_THEM; // Chuyen sang input vi tri
                    }
                }
                // Xu ly input Vi Tri
                else if (inputHoatDong == INPUT_VI_TRI_THEM) {
                    if (event.type == sf::Event::TextEntered) {
                        if (event.text.unicode < 128 && event.text.unicode != 8 && event.text.unicode != 9) {
                            char c = static_cast<char>(event.text.unicode);
                            if ((isalnum(c) || c == ' ' || c == '-') && viTriBanSaoCanThemStr.length() < MAX_VI_TRI_KE) {
                                viTriBanSaoCanThemStr += c;
                                if (loaiThongBao == 1) CapNhatThongBaoSFML("", 0);
                            }
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Backspace) {
                        if (!viTriBanSaoCanThemStr.empty()) {
                            viTriBanSaoCanThemStr.pop_back();
                            if (loaiThongBao == 1) CapNhatThongBaoSFML("", 0);
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    MaUI elementNhan = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);
                    if (elementNhan == NUT_XAC_NHAN_THEM_BS) {
                        ThucHienThemBanSao();
                    }
                    else if (elementNhan == NUT_HUY_THEM_BS) {
                        hienThiModalThemBS = false;
                        soLuongBanSaoCanThemStr = "";
                        viTriBanSaoCanThemStr = "";
                        inputHoatDong = KHONG_XAC_DINH;
                        CapNhatThongBaoSFML("Da huy them ban sao.", 0);
                    }
                    else if (elementNhan == INPUT_SO_LUONG_THEM) {
                        inputHoatDong = INPUT_SO_LUONG_THEM;
                    }
                    else if (elementNhan == INPUT_VI_TRI_THEM) {
                        inputHoatDong = INPUT_VI_TRI_THEM;
                    }
                    else {
                        inputHoatDong = KHONG_XAC_DINH;
                    }
                }
            }
            return; // Chan cac su kien khac
        }
        else if (hienThiModalBanSao) { // Xu ly rieng cho Modal Chi Tiet
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    MaUI elementNhan = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);

                    // Xu ly hop thoai xac nhan THANH LY
                    if (xacNhanThanhLy) {
                        if (elementNhan == NUT_XAC_NHAN_THANHLY) {
                            ThucHienThanhLySach(maSachCanThanhLy);
                            xacNhanThanhLy = false;
                            maSachCanThanhLy = "";
                        }
                        else if (elementNhan == NUT_HUY_THANHLY) {
                            xacNhanThanhLy = false;
                            maSachCanThanhLy = "";
                            CapNhatThongBaoSFML("Da huy thanh ly.", 0);
                        }
                        return;
                    }
                    
                    // Xu ly hop thoai xac nhan XOA BAN SAO
                    if (xacNhanXoaBanSao) {
                        if (elementNhan == NUT_XAC_NHAN_XOA_BANSAO) {
                            ThucHienXoaBanSao(maSachCanXoa);
                            xacNhanXoaBanSao = false;
                            maSachCanXoa = "";
                        }
                        else if (elementNhan == NUT_HUY_XOA_BANSAO) {
                            xacNhanXoaBanSao = false;
                            maSachCanXoa = "";
                            CapNhatThongBaoSFML("Da huy xoa ban sao.", 0);
                        }
                        return;
                    }

                    if (elementNhan == NUT_DONG_MODAL_CHITIET) {
                        hienThiModalBanSao = false;
                        idModalSachDuocChon = "";
                        trangModal = 1;
                        return;
                    }
                    
                    // Xu ly phan trang modal
                    if (elementNhan == NUT_MODAL_TRANG_TRUOC) {
                        if (trangModal > 1) {
                            trangModal--;
                        }
                        return;
                    }
                    if (elementNhan == NUT_MODAL_TRANG_SAU) {
                        // Tinh tong so trang
                        extern PTRDS dsDauSach[];
                        extern int soLuongDauSach;
                        PTRDS dauSachCheck = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, idModalSachDuocChon);
                        if (dauSachCheck) {
                            int tongBS = 0;
                            PTRDMS pC = dauSachCheck->dms;
                            while (pC) { tongBS++; pC = pC->next; }
                            int tongTrang = (tongBS + SO_SACH_MOI_TRANG_MODAL - 1) / SO_SACH_MOI_TRANG_MODAL;
                            if (trangModal < tongTrang) {
                                trangModal++;
                            }
                        }
                        return;
                    }

                    // Kiem tra click vao nut Thanh Ly hoac Xoa
                    for (int i = 0; i < soLuongNutModal; ++i) {
                        // Nut Thanh Ly
                        if (cacNutThanhLyTrongModal[i].khuVucNhanThanhLy.contains(event.mouseButton.x, event.mouseButton.y)) {
                            xacNhanThanhLy = true;
                            maSachCanThanhLy = cacNutThanhLyTrongModal[i].maSach;
                            return;
                        }
                        // Nut Xoa
                        if (cacNutThanhLyTrongModal[i].khuVucNhanXoa.contains(event.mouseButton.x, event.mouseButton.y)) {
                            xacNhanXoaBanSao = true;
                            maSachCanXoa = cacNutThanhLyTrongModal[i].maSach;
                            return;
                        }
                    }
                }
            }
            return; // Chan cac su kien khac
        }
    }

    // Code xu ly binh thuong (khi modal khong hien)
    // Xu ly nhap lieu van ban (go phim, xoa...)
    if (event.type == sf::Event::TextEntered || (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Backspace || event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Tab))) {
        if (inputHoatDong != KHONG_XAC_DINH) {
            XuLyTextInput(event);
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
                elementNhan == INPUT_SO_LUONG) {
                if (!(dangSua && elementNhan == INPUT_ISBN)) {
                    if (inputHoatDong != elementNhan) {
                        inputHoatDong = elementNhan;
                        CapNhatThongBaoSFML("", 0);
                    }
                    clickedInput = true;
                }
            }

            // Neu click ra ngoai o input thi deactive
            if (!clickedInput && inputHoatDong != KHONG_XAC_DINH) {
                inputHoatDong = KHONG_XAC_DINH;
                KhoiTaoStack(&undoStack);
                CapNhatThongBaoSFML("", 0);
            }

            // Xu ly click vao NUT (neu khong phai click vao input)
            if (!clickedInput) {
                switch (elementNhan) {
                // Nut Quay Ve Menu hoac Huy Sua
                case NUT_BACK:
                    if (dangSua) { // Neu dang sua thi Huy Sua
                        dangSua = false;
                        XoaFormNhapLieuSFML();
                        CapNhatThongBaoSFML("Da huy hieu chinh.", 0);
                    }
                    else {                                // Neu khong thi Quay Ve Menu
                        manHinhHienTai = MENU_CHINH; // Chuyen man hinh
                        // Reset cac trang thai cua man hinh Sach
                        dangSua = false;
                        xacNhanXoa = false;
                        inputHoatDong = KHONG_XAC_DINH;
                        isbnSachDuocChon = "";
                        cheDoXemHienTai = XEM_TIM_KIEM; // Reset che do xem
                        scrollOffsetYTheLoai = 0.f;     // Reset vi tri cuon
                        CapNhatThongBaoSFML("", 0);     // Xoa thong bao
                    }
                    break; // Ket thuc case NUT_BACK
                // Nut Tim Kiem
                case NUT_TIM:
                    inputHoatDong = KHONG_XAC_DINH;
                    cheDoXemHienTai = XEM_TIM_KIEM; // Nhan Tim thi luon chuyen ve che do Tim Kiem
                    scrollOffsetYTheLoai = 0.f;
                    ThucHienTimKiemNoiBo();
                    break;
                // Nut Xoa Tim Kiem (Reset)
                case NUT_XOA_TIM:
                    chuoiTimKiem = "";
                    inputHoatDong = KHONG_XAC_DINH;
                    cheDoXemHienTai = XEM_TIM_KIEM; // Reset cung chuyen ve che do Tim Kiem
                    scrollOffsetYTheLoai = 0.f;
                    ThucHienTimKiemNoiBo();
                    break;
                // Nut Them Sach / Luu Thay Doi
                case NUT_THEM_SACH:
                    inputHoatDong = KHONG_XAC_DINH;
                    ThucHienThemHoacSuaSachSFML();
                    break;

                case NUT_CHUYEN_CHE_DO_XEM:
                    if (cheDoXemHienTai == XEM_TIM_KIEM) {
                        cheDoXemHienTai = XEM_THEO_THE_LOAI;
                        scrollOffsetYTheLoai = 0.f;
                        // Reset trang thai khong can thiet cho che do xem the loai
                        isbnSachDuocChon = "";
                        dangSua = false;
                        xacNhanXoa = false;
                        inputHoatDong = KHONG_XAC_DINH;
                        CapNhatThongBaoSFML("Dang xem theo the loai.", 0);
                    }
                    else {
                        cheDoXemHienTai = XEM_TIM_KIEM;
                        scrollOffsetYTheLoai = 0.f;
                        ThucHienTimKiemNoiBo(); // Tai lai danh sach tim kiem/tat ca
                        // Thong bao da duoc cap nhat trong ThucHienTimKiemNoiBo
                    }
                    break;

                case NUT_TRANG_TRUOC:
                    if (cheDoXemHienTai == XEM_TIM_KIEM && trangHienTai > 1)
                        trangHienTai--;
                    break;
                case NUT_TRANG_SAU:
                    if (cheDoXemHienTai == XEM_TIM_KIEM && trangHienTai < tongSoTrang)
                        trangHienTai++;
                    break;
                case NUT_THEM_BAN_SAO:
                    if (cheDoXemHienTai == XEM_TIM_KIEM && !isbnSachDuocChon.empty()) {
                        hienThiModalThemBS = true;
                        soLuongBanSaoCanThemStr = "";
                        viTriBanSaoCanThemStr = ""; // Reset vi tri
                        inputHoatDong = INPUT_SO_LUONG_THEM;
                        CapNhatThongBaoSFML("Nhap so luong va vi tri ban sao can them.", 0);
                    }
                    else if (cheDoXemHienTai == XEM_THEO_THE_LOAI) {
                        CapNhatThongBaoSFML("Chuc nang nay khong dung duoc o che do xem theo the loai.", 1);
                    }
                    else {
                        CapNhatThongBaoSFML("Loi: Vui long chon sach truoc!", 1);
                    }
                    break;
                case NUT_CHI_TIET_SACH:
                    if (cheDoXemHienTai == XEM_TIM_KIEM && !isbnSachDuocChon.empty()) {
                        hienThiModalBanSao = true;
                        idModalSachDuocChon = isbnSachDuocChon;
                        trangModal = 1;
                    }
                    else if (cheDoXemHienTai == XEM_THEO_THE_LOAI) {
                        CapNhatThongBaoSFML("Chuc nang nay khong dung duoc o che do xem theo the loai.", 1);
                    }
                    else {
                        CapNhatThongBaoSFML("Loi: Vui long chon sach truoc!", 1);
                    }
                    break;
                case NUT_SUA:
                    if (cheDoXemHienTai == XEM_TIM_KIEM && !isbnSachDuocChon.empty()) {
                        DienFormVoiSachDuocChon();
                        dangSua = true;
                        xacNhanXoa = false;
                        inputHoatDong = INPUT_TEN_SACH;
                        CapNhatThongBaoSFML("Dang hieu chinh sach...", 0);
                    }
                    else if (cheDoXemHienTai == XEM_THEO_THE_LOAI) {
                        CapNhatThongBaoSFML("Chuc nang nay khong dung duoc o che do xem theo the loai.", 1);
                    }
                    else
                        CapNhatThongBaoSFML("Vui long chon mot dau sach de sua!", 1);
                    break;
                case NUT_XOA:
                    if (cheDoXemHienTai == XEM_TIM_KIEM) { // chi kiem tra xoa o che do tim kiem
                        if (isbnSachDuocChon.empty()) {
                            CapNhatThongBaoSFML("Loi: Vui long chon mot dau sach de xoa!", 1);
                        }
                        else {
                            extern PTRDS dsDauSach[];
                            extern int soLuongDauSach;
                            PTRDS sachCanXoa = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbnSachDuocChon);

                            if (sachCanXoa && sachCanXoa->dms != nullptr) { // kiem tra con ban sao
                                CapNhatThongBaoSFML("Loi: Khong the xoa '" + sachCanXoa->tenSach + "' vi van con ban sao.", 1);
                            }
                            else if (sachCanXoa) {
                                xacNhanXoa = true; // moi hien xac nhan
                                dangSua = false;
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
                    if (xacNhanXoa && !isbnSachDuocChon.empty())
                        ThucHienXoaSachSFML();
                    xacNhanXoa = false;
                    break;
                case NUT_HUY_XOA: // Giu nguyen
                    xacNhanXoa = false;
                    CapNhatThongBaoSFML("Da huy thao tac xoa.", 0);
                    break;
                case NUT_HUY_CHON:
                    if (cheDoXemHienTai == XEM_TIM_KIEM) { // chi huy chon o che do tim kiem
                        isbnSachDuocChon = "";
                        dangSua = false;
                        xacNhanXoa = false;
                        XoaFormNhapLieuSFML();
                        CapNhatThongBaoSFML("Da huy chon.", 0);
                    }
                    break;
                case HANG_SACH:
                    if (cheDoXemHienTai == XEM_TIM_KIEM) { // chi xu ly double click o che do tim kiem
                        float contentY = BANG_Y + 90 + 35;
                        float rowHeight = 30.f;
                        int startIndex = (trangHienTai - 1) * SACH_MOI_TRANG;
                        int rowIndex = static_cast<int>((event.mouseButton.y - contentY) / rowHeight);
                        int actualIndex = startIndex + rowIndex;

                        if (actualIndex >= 0 && actualIndex < soLuongKetQuaTimKiem && ketQuaTimKiem[actualIndex].sach) {
                            isbnRowClicked = ketQuaTimKiem[actualIndex].sach->ISBN;
                        }
                        else {
                            isbnRowClicked = "";
                        }

                        if (isbnRowClicked.empty())
                            break;
                        float thoiGianTroiQua = doubleClickClock.getElapsedTime().asSeconds();
                        if (thoiGianTroiQua < THOI_GIAN_DOUBLE_CLICK && isbnRowClicked == isbnClickCuoi) {
                            isbnSachDuocChon = isbnRowClicked;
                            dangSua = false;
                            xacNhanXoa = false;
                            CapNhatThongBaoSFML("Da chon: " + ketQuaTimKiem[actualIndex].sach->tenSach, 0);
                            inputHoatDong = KHONG_XAC_DINH;
                            XoaFormNhapLieuSFML();
                            isbnClickCuoi = "";
                        }
                        else {
                            isbnClickCuoi = isbnRowClicked;
                            doubleClickClock.restart();
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
        else if (dangSua) {
            dangSua = false;
            XoaFormNhapLieuSFML();
            CapNhatThongBaoSFML("Da huy hieu chinh.", 0);
        }
        else if (xacNhanXoa) {
            xacNhanXoa = false;
            CapNhatThongBaoSFML("Da huy thao tac xoa.", 0);
        }
        else {
            manHinhHienTai = MENU_CHINH;
            cheDoXemHienTai = XEM_TIM_KIEM; // Reset che do xem khi thoat
            scrollOffsetYTheLoai = 0.f;
            CapNhatThongBaoSFML("", 0);
        }
    }
}


// Ham xu ly nhap lieu text 
static void XuLyTextInput(sf::Event event) {
    // Kiem tra xem co input nao dang active khong
    if (inputHoatDong == KHONG_XAC_DINH) return;
    // Khong xu ly Tab neu la o nhap so luong trong modal Them BS
    if(inputHoatDong == INPUT_SO_LUONG_THEM && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) return;

    // Khoi tao stack lan dau (neu chua)
    if (!stackDaKhoiTao) {
        KhoiTaoStack(&undoStack);
        stackDaKhoiTao = true;
    }

    std::string* targetString = nullptr; // Con tro den chuoi can sua
    int maxLen = 0;
    bool chiNhanSo = false;
    std::string fieldName = "";

    // Gan con tro chuoi vao input dang active
    switch (inputHoatDong) {
        case INPUT_TIM_SACH: targetString = &chuoiTimKiem; maxLen = 50; fieldName = "Tim kiem"; break;
        case INPUT_ISBN:     targetString = &chuoiISBN; maxLen = 13; chiNhanSo = true; fieldName = "ISBN"; break;
        case INPUT_TEN_SACH: targetString = &chuoiTenSach; maxLen = MAX_TEN_SACH; fieldName = "Ten sach"; break;
        case INPUT_SO_TRANG: targetString = &chuoiSoTrang; maxLen = 5; chiNhanSo = true; fieldName = "So trang"; break;
        case INPUT_TAC_GIA:  targetString = &chuoiTacGia; maxLen = MAX_TAC_GIA; fieldName = "Tac gia"; break;
        case INPUT_NAM_XB:   targetString = &chuoiNamXB; maxLen = 4; chiNhanSo = true; fieldName = "Nam XB"; break;
        case INPUT_THE_LOAI: targetString = &chuoiTheLoai; maxLen = MAX_THE_LOAI; fieldName = "The loai"; break;
        case INPUT_SO_LUONG: targetString = &chuoiSoLuong; maxLen = 3; chiNhanSo = true; fieldName = "So luong"; break;
        case INPUT_SO_LUONG_THEM: targetString = &soLuongBanSaoCanThemStr; maxLen = 3; chiNhanSo = true; fieldName = "So luong them"; break;
        default: return; // Thoat neu khong phai input hop le
    }

    // Xu ly Ctrl + Z (Undo)
    if (event.type == sf::Event::KeyPressed && event.key.control && event.key.code == sf::Keyboard::Z) {
        if (!KiemTraStackRong(&undoStack) && targetString != nullptr) { // Kiem tra stack khong rong
            *targetString = PopStack(&undoStack); // Pop trang thai cu va gan lai
            if (loaiThongBao == 1) CapNhatThongBaoSFML("", 0); // Xoa thong bao loi (neu co)
        }
        return; // Da xu ly Undo, khong lam gi them
    }

    bool changed = false; // Co de kiem tra xem noi dung co thay doi khong
    std::string currentState = (targetString != nullptr) ? *targetString : ""; // Luu noi dung hien tai TRUOC KHI THAY DOI

    // Xu ly go phim
    if (event.type == sf::Event::TextEntered) {
        // Chi lay ky tu ASCII, khong lay Backspace, Enter, Tab
        if (event.text.unicode < 128 && event.text.unicode != 8 && event.text.unicode != 13 && event.text.unicode != 9) {
            char enteredChar = static_cast<char>(event.text.unicode);
            if (targetString->length() < static_cast<size_t>(maxLen)) { // Kiem tra do dai toi da
                bool allowChar = false; // Co cho phep them ky tu nay khong
                if (chiNhanSo) { // Neu la o chi nhan so
                    if (enteredChar >= '0' && enteredChar <= '9') allowChar = true;
                } else { // Neu la o nhan chu va so
                    // Cho phep chu, so, khoang trang va 1 so ky tu dac biet
                    if (isalnum(enteredChar) || enteredChar == ' ' || enteredChar == '+' || enteredChar == '#' || enteredChar == '-' || enteredChar == '.' || enteredChar == '_') {
                        // Ky tu dau tien phai la chu cai
                        if (targetString->empty() && !isalpha(enteredChar)) {
                            allowChar = false;
                            CapNhatThongBaoSFML("Loi: Ky tu dau phai la chu cai!", 1);
                        } else allowChar = true;
                    }
                }
                // Neu ky tu hop le
                if (allowChar) {
                    PushStack(&undoStack, currentState); // Push trang thai cu vao stack TRUOC KHI them ky tu moi
                    *targetString += enteredChar;        // Them ky tu vao chuoi
                    changed = true;                      // Danh dau da thay doi
                    if(loaiThongBao == 1) CapNhatThongBaoSFML("", 0); // Xoa loi neu co
                }
            }
        }
    } else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Backspace) { // Xu ly Backspace
        if (!targetString->empty()) {            // Chi xoa neu chuoi khong rong
            PushStack(&undoStack, currentState); // Push trang thai CU vao stack TRUOC KHI xoa
            targetString->pop_back();            // Xoa ky tu cuoi
            changed = true;                      // Danh dau da thay doi
            if(loaiThongBao == 1) CapNhatThongBaoSFML("", 0); // Xoa loi neu co
        }
    }

    // Kiem tra loi ngay khi dang go (realtime validation)
    // Khong can kiem tra realtime cho o tim kiem va o so luong them
    if (changed && inputHoatDong != INPUT_TIM_SACH && inputHoatDong != INPUT_SO_LUONG_THEM) {
         std::string loi = "";
        std::string tempStr = *targetString;
        switch (inputHoatDong) {
            case INPUT_ISBN:
                loi = KiemTraChuoiRong(tempStr, fieldName);
                if (loi.empty() && tempStr.length() > 0) {
                    for(char c : tempStr) { if (!isdigit(c)) { loi = "ISBN chi duoc chua so!"; break; } }
                }
                break;
            case INPUT_TEN_SACH: case INPUT_TAC_GIA: case INPUT_THE_LOAI:
                loi = KiemTraChuoiVaDodai(tempStr, fieldName, maxLen);
                if (loi.empty() && !tempStr.empty() && !isalpha(tempStr[0])) loi = "Ky tu dau phai la chu cai!";
                break;
            case INPUT_SO_TRANG: case INPUT_NAM_XB: case INPUT_SO_LUONG:
                loi = KiemTraChuoiRong(tempStr, fieldName);
                if (loi.empty() && !tempStr.empty()) {
                    for(char c : tempStr) { if (!isdigit(c)) { loi = fieldName + " phai la so!"; break; } }
                    if (loi.empty()) {
                        int value;
                        if (chuyenChuoiThanhSoNguyen(tempStr, value, true)) {
                            if (inputHoatDong == INPUT_SO_TRANG && (value <= 0 || value > MAX_SO_TRANG)) loi = "So trang phai tu 1-" + std::to_string(MAX_SO_TRANG) + "!";
                            else if (inputHoatDong == INPUT_NAM_XB && tempStr.length() == 4) loi = KiemTraNamXuatBan(value);
                            else if (inputHoatDong == INPUT_SO_LUONG && (value <= 0 || value > MAX_BAN_SAO)) loi = "So luong phai tu 1-" + std::to_string(MAX_BAN_SAO) + "!";
                        } else loi = fieldName + " khong hop le!";
                    }
                }
                break;
            default: break;
        }
        if (!loi.empty()) CapNhatThongBaoSFML(loi, 1);
        else if (loaiThongBao == 1) CapNhatThongBaoSFML("", 0);
    }

    // Xu ly nhan Enter/Tab de nhay input (khong ap dung cho modal)
    if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Tab) && inputHoatDong != INPUT_SO_LUONG_THEM) {
         std::string loiCuoi = "";
        std::string tempStrCuoi = *targetString;
        tempStrCuoi = CatKhoangTrang(tempStrCuoi);
        switch (inputHoatDong) {
             case INPUT_ISBN:
                 loiCuoi = KiemTraChuoiRong(tempStrCuoi, fieldName);
                 if (loiCuoi.empty()) {
                     std::string isbnChuanTam;
                     std::string loiChuan = ChuanHoaISBNFile(tempStrCuoi, isbnChuanTam);
                     if (!loiChuan.empty()) loiCuoi = loiChuan;
                     else if (!dangSua && !KiemTraTrungISBN(isbnChuanTam).empty()) loiCuoi = "ISBN nay da ton tai!";
                 }
                 break;
             case INPUT_SO_TRANG: {
                 int v;
                 if (!chuyenChuoiThanhSoNguyen(tempStrCuoi, v, true) || v <= 0 || v > MAX_SO_TRANG ) loiCuoi = "So trang khong hop le (1-" + std::to_string(MAX_SO_TRANG) + ")!";
                 break;
             }
             default: break;
        }

        if (loiCuoi.empty()) {
             KhoiTaoStack(&undoStack); // Reset Stack khi chuyen input
             if (inputHoatDong >= INPUT_TIM_SACH && inputHoatDong < INPUT_SO_LUONG) {
                 inputHoatDong = static_cast<MaUI>(inputHoatDong + 1);
                 CapNhatThongBaoSFML("", 0);
             } else if (inputHoatDong == INPUT_SO_LUONG) {
                 inputHoatDong = KHONG_XAC_DINH;
                 if (event.key.code == sf::Keyboard::Enter) ThucHienThemHoacSuaSachSFML();
             } else {
                  inputHoatDong = KHONG_XAC_DINH;
             }
        } else CapNhatThongBaoSFML(loiCuoi, 1);
    }
}


// Cap nhat so trang hien thi
static void CapNhatPhanTrangSFML() {
    int totalItems = soLuongKetQuaTimKiem;
    tongSoTrang = (totalItems + SACH_MOI_TRANG - 1) / SACH_MOI_TRANG;
    if (tongSoTrang == 0)
        tongSoTrang = 1; // It nhat 1 trang
    if (trangHienTai > tongSoTrang)
        trangHienTai = tongSoTrang; // Khong vuot qua trang cuoi
}

// Goi backend de tim kiem/lay tat ca sach
static void ThucHienTimKiemNoiBo() {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    soLuongKetQuaTimKiem = 0;                               // Reset ket qua cu
    std::string tuKhoa = ChuanHoaKhoangTrang(chuoiTimKiem); // Lay tu khoa tu input
    if (tuKhoa.empty()) { // Neu khong nhap gi -> Hien thi tat ca
        for (int i = 0; i < soLuongDauSach; ++i) {
            if (soLuongKetQuaTimKiem < MAX_DAUSACH) {
                ketQuaTimKiem[soLuongKetQuaTimKiem].sach = dsDauSach[i];
                ketQuaTimKiem[soLuongKetQuaTimKiem].loaiKhop = 0;
                soLuongKetQuaTimKiem++;
            }
            else
                break; // Tranh vuot qua mang ket qua
        }
        
        // Sap xep tat ca sach theo ten (A->Z)
        if (soLuongKetQuaTimKiem > 1) {
            PTRDS tempArr[MAX_DAUSACH];
            for (int i = 0; i < soLuongKetQuaTimKiem; ++i) {
                tempArr[i] = ketQuaTimKiem[i].sach;
            }
            sapXepDauSachTheoTen(tempArr, 0, soLuongKetQuaTimKiem - 1);
            for (int i = 0; i < soLuongKetQuaTimKiem; ++i) {
                ketQuaTimKiem[i].sach = tempArr[i];
            }
        }
    }
    else { // Neu co tu khoa -> Goi ham tim kiem logic
        soLuongKetQuaTimKiem = timKiemLogic(dsDauSach, soLuongDauSach, tuKhoa, ketQuaTimKiem);
    }
    trangHienTai = 1;       // Luon ve trang 1 sau khi tim kiem
    CapNhatPhanTrangSFML(); // Tinh lai tong so trang
    // Reset trang thai sau tim kiem
    isbnSachDuocChon = "";
    dangSua = false;
    xacNhanXoa = false;

    // Chan thong bao tim kiem (de giu thong bao "Them thanh cong")
    if (vuaThucHienThanhCong == false) {
        if (soLuongKetQuaTimKiem > 0 || tuKhoa.empty()) {
            if (tuKhoa.empty() && soLuongDauSach > 0)
                CapNhatThongBaoSFML("Hien thi tat ca " + std::to_string(soLuongKetQuaTimKiem) + " dau sach.", 0);
            else if (soLuongKetQuaTimKiem > 0)
                CapNhatThongBaoSFML("Tim thay " + std::to_string(soLuongKetQuaTimKiem) + " ket qua.", 0);
            else if (soLuongDauSach == 0 && tuKhoa.empty())
                CapNhatThongBaoSFML("Danh sach rong.", 0);
            else
                CapNhatThongBaoSFML("Khong tim thay ket qua cho '" + tuKhoa + "'.", 1);
        }
        else {
            CapNhatThongBaoSFML("Khong tim thay ket qua cho '" + tuKhoa + "'.", 1);
        }
    }
    vuaThucHienThanhCong = false; // Reset co lai
}

// Ham khoi tao (public) - Goi khi chuyen sang man hinh Sach
void KhoiTaoManHinhSach() {
    ThucHienTimKiemNoiBo(); // Goi ham tim kiem noi bo de load danh sach
}

// Xoa trang noi dung form
static void XoaFormNhapLieuSFML() {
    chuoiISBN = "";
    chuoiTenSach = "";
    chuoiSoTrang = "";
    chuoiTacGia = "";
    chuoiNamXB = "";
    chuoiTheLoai = "";
    chuoiSoLuong = "";
}

// Dien du lieu sach da chon vao form (khi nhan Sua)
static void DienFormVoiSachDuocChon() {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    PTRDS sach = TimDauSachTheoISBN(dsDauSach, soLuongDauSach, isbnSachDuocChon);
    if (sach) {
        chuoiISBN = sach->ISBN;
        chuoiTenSach = sach->tenSach;
        chuoiSoTrang = std::to_string(sach->soTrang);
        chuoiTacGia = sach->tacGia;
        chuoiNamXB = std::to_string(sach->namXuatBan);
        chuoiTheLoai = sach->theLoai;
    }
    else {
        CapNhatThongBaoSFML("Loi: Khong tim thay sach de sua!", 1);
        XoaFormNhapLieuSFML();
        isbnSachDuocChon = ""; // Huy chon neu loi
    }
}

// Goi backend de Them Dau Sach / Cap Nhat Dau Sach
static void ThucHienThemHoacSuaSachSFML() {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    extern bool duLieuDaThayDoi;
    std::string loi;
    std::string isbnChuan = chuoiISBN;
    int soTrang = 0;
    int namXB = 0;
    int soLuong = 0;

    // Kiem tra loi chung cho ca Them va Sua
    loi = KiemTraChuoiVaDodai(chuoiTenSach, "Ten sach", MAX_TEN_SACH);
    if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        inputHoatDong = INPUT_TEN_SACH;
        return;
    }
    if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(chuoiSoTrang), soTrang, true) || soTrang <= 0 || soTrang > MAX_SO_TRANG) {
        CapNhatThongBaoSFML("Loi: So trang (1-" + std::to_string(MAX_SO_TRANG) + ")!", 1);
        inputHoatDong = INPUT_SO_TRANG;
        return;
    }
    loi = KiemTraChuoiVaDodai(chuoiTacGia, "Tac gia", MAX_TAC_GIA);
    if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        inputHoatDong = INPUT_TAC_GIA;
        return;
    }
    if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(chuoiNamXB), namXB, true)) {
        CapNhatThongBaoSFML("Loi: Nam xuat ban phai la so!", 1);
        inputHoatDong = INPUT_NAM_XB;
        return;
    }
    loi = KiemTraNamXuatBan(namXB);
    if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        inputHoatDong = INPUT_NAM_XB;
        return;
    }
    loi = KiemTraChuoiVaDodai(chuoiTheLoai, "The loai", MAX_THE_LOAI);
    if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        inputHoatDong = INPUT_THE_LOAI;
        return;
    }

    if (!dangSua) {
        // Kiem tra loi rieng cho Them (ISBN, So Luong)
        loi = KiemTraChuoiRong(chuoiISBN, "ISBN");
        if (!loi.empty()) {
            CapNhatThongBaoSFML(loi, 1);
            inputHoatDong = INPUT_ISBN;
            return;
        }

        loi = ChuanHoaISBNFile(chuoiISBN, isbnChuan); // Chuan hoa + Kiem tra 10/13 so
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

        if (!chuyenChuoiThanhSoNguyen(CatKhoangTrang(chuoiSoLuong), soLuong, true) || soLuong <= 0 || soLuong > MAX_BAN_SAO) {
            CapNhatThongBaoSFML("Loi: So luong (1-" + std::to_string(MAX_BAN_SAO) + ")?", 1);
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
        bool ok = themDauSach(dsDauSach, soLuongDauSach, isbnChuan, chuoiTenSach, soTrang, chuoiTacGia, namXB, chuoiTheLoai, true); // true = an lang (khong cout)
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
                std::string thongBao = "Them DS OK, nhung chi them duoc " + std::to_string(themThanhCong) + "/" + std::to_string(soLuong) + " ban sao. Loi: " + loiTrongLoop;
                CapNhatThongBaoSFML(thongBao, 1); // Loai 1 = Mau do
            }
            else {
                // Bao thanh cong len UI
                CapNhatThongBaoSFML("Them thanh cong: " + ChuyenThanhTitleCase(chuoiTenSach), 2);
                vuaThucHienThanhCong = true;                                // Dat co de khong bi de thong bao
            }

            // Reset form, tai lai bang
            XoaFormNhapLieuSFML();
            ThucHienTimKiemNoiBo();
            inputHoatDong = KHONG_XAC_DINH;
        }
        else {
            CapNhatThongBaoSFML("Loi: Khong tim thay DauSach vua them?!", 1);
        }
    }
    else { //
        // Goi backend cap nhat dau sach
        loi = CapNhatDauSach(dsDauSach, soLuongDauSach, isbnSachDuocChon, chuoiTenSach, soTrang, chuoiTacGia, namXB, chuoiTheLoai);

        // Bao ket qua len UI
        if (loi.empty()) {
            CapNhatThongBaoSFML("Cap nhat thanh cong: " + ChuyenThanhTitleCase(chuoiTenSach), 2);
            vuaThucHienThanhCong = true; // Dat co
            // Reset form, tai lai bang, bo chon sach
            XoaFormNhapLieuSFML();
            ThucHienTimKiemNoiBo();
            dangSua = false;
            inputHoatDong = KHONG_XAC_DINH;
            isbnSachDuocChon = "";
        }
        else {
            CapNhatThongBaoSFML(loi, 1);
        } // Bao loi neu co
    }
}

// Goi backend de Xoa Dau Sach
static void ThucHienXoaSachSFML() {
    extern PTRDS dsDauSach[];
    extern int soLuongDauSach;
    extern bool duLieuDaThayDoi;
    (void)duLieuDaThayDoi; // Danh dau khong dung truc tiep

    if (isbnSachDuocChon.empty()) {
        CapNhatThongBaoSFML("Loi: Chua chon dau sach de xoa!", 1);
        return;
    }

    // Goi backend xoa
    std::string loi = XoaDauSachTheoISBN(dsDauSach, soLuongDauSach, isbnSachDuocChon); // Ham tra ve "" hoac chuoi loi

    // Bao ket qua len UI
    if (loi.empty()) {
        CapNhatThongBaoSFML("Xoa dau sach thanh cong!", 2);
        isbnSachDuocChon = "";  // Bo chon sach da xoa
        ThucHienTimKiemNoiBo(); // Tai lai bang
    }
    else {
        CapNhatThongBaoSFML(loi, 1);
    } // Bao loi neu co
    xacNhanXoa = false; // An hop thoai xac nhan
}
