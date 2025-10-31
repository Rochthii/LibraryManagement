
#include "include/GiaoDienDocGiaMuonTra.h"
#include "include/TienIchGiaoDien.h"
#include "include/QuanLyDocGia.h"
#include "include/QuanLyMuonTra.h"
#include "include/ThongBao.h"
#include "include/NgayThang.h"
#include <cstdlib>
#include <cstring>

// Trang thai tab
enum Tab { TAB_DOCGIA = 0, TAB_MUONTRA = 1 };
static Tab tabHienTai = TAB_DOCGIA;

// Cac bien UI chung / doc gia
static PTRDG rootDocGia_UI = nullptr;
static std::string input_MaThe = "";
static std::string input_Ho = "";
static std::string input_Ten = "";
static bool input_Phai = false;
static int input_TrangThai = 1;
static bool showDanhSachDocGia = false;

// Cac bien UI muon-tra
static std::string input_MaSach = "";
static bool chk_MatSach = false;
static bool showDanhSachMuon = false;
static bool showDanhSachQuaHan = false;

// Khoi tao (tai du lieu)
void KhoiTaoManHinhDocGiaMuonTra(sf::RenderWindow& window, const sf::Font& font) {
    (void)window; (void)font;
    // Tai doc gia vao bien local UI (neu ban muon dung root toan cuc thi chuyen thong tin)
    rootDocGia_UI = loadDocGia();
    input_MaThe.clear();
    input_Ho.clear();
    input_Ten.clear();
    input_Phai = false;
    input_TrangThai = 1;
    input_MaSach.clear();
    chk_MatSach = false;
    showDanhSachDocGia = false;
    showDanhSachMuon = false;
    showDanhSachQuaHan = false;
    tabHienTai = TAB_DOCGIA;
}

// Ham ve header tab
static void VeHeaderTab(sf::RenderWindow& window, const sf::Font& font) {
    float y = 20.f;
    sf::Text t = TaoVanBan(font, "QUAN LY THU VIEN - DOC GIA & MUON-TRA", 24, MAU_TIEU_DE);
    CanGiuaGocXVanBan(t);
    t.setPosition(CHIEU_RONG / 2.0f, y);
    window.draw(t);

    // Tabs
    float tx = 80.f;
    TaoNut(font, (MaUI)(900), tx, 60.f, 180.f, 40.f, "DOC GIA", MAU_NEN_NUT, MAU_CHU_NUT);
    tx += 190.f;
    TaoNut(font, (MaUI)(901), tx, 60.f, 180.f, 40.f, "MUON-TRA", MAU_NEN_NUT, MAU_CHU_NUT);
    // Note: homepage will map those ids to actions in event handler
}

// Vẽ phần DocGia
static void VeTabDocGia(sf::RenderWindow& window, const sf::Font& font) {
    float x = 60.f, y = 130.f;
    float w = 300.f, h = 42.f, gap = 12.f;

    TaoInput(font, INPUT_MA_THE, x, y, w, h, "Ma the (de tim/sua/xoa)", input_MaThe, MAU_NEN_INPUT, MAU_CHU_INPUT);
    y += h + gap;
    TaoInput(font, INPUT_HO_DOC_GIA, x, y, w, h, "Ho", input_Ho, MAU_NEN_INPUT, MAU_CHU_INPUT);
    y += h + gap;
    TaoInput(font, INPUT_TEN_DOC_GIA, x, y, w, h, "Ten", input_Ten, MAU_NEN_INPUT, MAU_CHU_INPUT);
    y += h + gap;

    TaoNut(font, NUT_PHAI_DOC_GIA, x, y, 120.f, h, input_Phai ? "Nu" : "Nam", MAU_NEN_NUT, MAU_CHU_NUT);
    TaoNut(font, NUT_TRANG_THAI_DOC_GIA, x + 140.f, y, 160.f, h, (input_TrangThai == 1) ? "Hoat dong" : "Khoa", MAU_NEN_NUT, MAU_CHU_NUT);

    // action buttons
    float bx = 450.f, by = 150.f, bw = 220.f, bh = 50.f, ky = 14.f;
    TaoNut(font, NUT_THEM_DOC_GIA, bx, by, bw, bh, "THEM", MAU_NEN_NUT, MAU_CHU_NUT);
    by += bh + ky;
    TaoNut(font, NUT_HIEU_CHINH_DOC_GIA, bx, by, bw, bh, "HIEU CHINH", MAU_NEN_NUT, MAU_CHU_NUT);
    by += bh + ky;
    TaoNut(font, NUT_XOA_DOC_GIA, bx, by, bw, bh, "XOA", MAU_NEN_NUT, MAU_CHU_NUT);
    by += bh + ky;
    TaoNut(font, NUT_LIET_KE_DOC_GIA, bx, by, bw, bh, showDanhSachDocGia ? "AN DANH SACH" : "LIET KE", MAU_NEN_NUT, MAU_CHU_NUT);

    // Bang danh sach doc gia (neu show)
    if (showDanhSachDocGia) {
        float tx = 60.f, ty = 380.f;
        sf::Text hdr = TaoVanBan(font, "MaThe | Ho | Ten | Phai | TrangThai", 18, MAU_TIEU_DE);
        hdr.setPosition(tx, ty);
        window.draw(hdr);
        ty += 28.f;

        DocGiaDonGian arr[200];
        int n = 0;
        LayDanhSachDocGia(rootDocGia_UI, arr, n, 200);
        int show = n < 12 ? n : 12;
        for (int i = 0; i < show; ++i) {
            std::string line = std::to_string(arr[i].MaThe) + " | " + arr[i].Ho + " | " + arr[i].Ten + " | " +
                (arr[i].Phai ? "Nu" : "Nam") + " | " + (arr[i].TrangThai ? "Hoat dong" : "Khoa");
            sf::Text tline = TaoVanBan(font, line, 16, MAU_CHU);
            tline.setPosition(tx, ty + i * 24.f);
            window.draw(tline);
        }
    }
}

// Vẽ phần MuonTra
static void VeTabMuonTra(sf::RenderWindow& window, const sf::Font& font) {
    float x = 60.f, y = 130.f;
    float w = 320.f, h = 42.f, gap = 12.f;

    TaoInput(font, INPUT_MA_THE, x, y, w, h, "Ma the", input_MaThe, MAU_NEN_INPUT, MAU_CHU_INPUT);
    TaoInput(font, INPUT_MA_SACH, x + 340.f, y, 320.f, h, "Ma sach", input_MaSach, MAU_NEN_INPUT, MAU_CHU_INPUT);
    y += h + gap;

    TaoNut(font, NUT_MUON_SACH, x, y, 180.f, 48.f, "MUON SACH", MAU_NEN_NUT, MAU_CHU_NUT);
    TaoNut(font, NUT_TRA_SACH, x + 200.f, y, 180.f, 48.f, "TRA SACH", MAU_NEN_NUT, MAU_CHU_NUT);
    TaoNut(font, NUT_MAT_SACH, x + 420.f, y, 160.f, 48.f, chk_MatSach ? "MAT (ON)" : "MAT (OFF)", MAU_NEN_NUT, MAU_CHU_NUT);
    TaoNut(font, NUT_LIET_KE_SACH_DANG_MUON, x + 600.f, y, 220.f, 48.f, showDanhSachMuon ? "AN LIEt KE" : "LIET KE SACH", MAU_NEN_NUT, MAU_CHU_NUT);
    y += h + gap;

    TaoNut(font, NUT_LIET_KE_DOC_GIA_QUA_HAN, x, y, 300.f, 48.f, showDanhSachQuaHan ? "AN QUA HAN" : "LIET KE QUA HAN", MAU_NEN_NUT, MAU_CHU_NUT);

    // Bang danh sach sach dang muon cua doc gia (neu showDanhSachMuon)
    if (showDanhSachMuon) {
        float tx = 60.f, ty = 360.f;
        sf::Text hdr = TaoVanBan(font, "MaSach | NgayMuon | TrangThai", 18, MAU_TIEU_DE);
        hdr.setPosition(tx, ty);
        window.draw(hdr);
        ty += 28.f;

        MUONTRA arr[100];
        int cnt = lietKeSachDangMuon(rootDocGia_UI, input_MaThe.empty() ? -1 : std::atoi(input_MaThe.c_str()), arr, 100);
        int show = cnt < 12 ? cnt : 12;
        for (int i = 0; i < show; ++i) {
            std::string tt = arr[i].TrangThai == 0 ? "Dang muon" : (arr[i].TrangThai == 1 ? "Da tra" : "Mat");
            std::string line = arr[i].MaSach + " | " + arr[i].NgayMuon + " | " + tt;
            sf::Text tline = TaoVanBan(font, line, 16, MAU_CHU);
            tline.setPosition(tx, ty + i * 24.f);
            window.draw(tline);
        }
    }

    // Bang danh sach doc gia qua han (neu showDanhSachQuaHan)
    if (showDanhSachQuaHan) {
        float tx = 60.f, ty = 360.f;
        sf::Text hdr = TaoVanBan(font, "MaThe | Ho Ten | MaSach | NgayQuaHan (ngay)", 18, MAU_TIEU_DE);
        hdr.setPosition(tx, ty);
        window.draw(hdr);
        ty += 28.f;

        int arrMa[200];
        int cnt = lietKeDocGiaQuaHan(rootDocGia_UI, arrMa, 200, layNgayHienTai());
        int show = cnt < 12 ? cnt : 12;
        for (int i = 0; i < show; ++i) {
            PTRDG p = timDocGia(rootDocGia_UI, arrMa[i]);
            if (!p) continue;
            // Tim cac muon tra qua han de lay ma sach va so ngay
            MUONTRA* q = p->data.dsmt;
            int soNgayMax = 0;
            std::string maSachMax = "";
            while (q) {
                if (q->data.TrangThai == 0) {
                    int days = tinhKhoangCachNgay(q->data.NgayMuon, layNgayHienTai());
                    if (days > soNgayMax) { soNgayMax = days; maSachMax = q->data.MaSach; }
                }
                q = q->next;
            }
            std::string line = std::to_string(p->data.MaThe) + " | " + p->data.Ho + " " + p->data.Ten + " | " +
                maSachMax + " | " + std::to_string((soNgayMax > 7) ? (soNgayMax - 7) : 0);
            sf::Text tline = TaoVanBan(font, line, 16, MAU_CHU);
            tline.setPosition(tx, ty + i * 24.f);
            window.draw(tline);
        }
    }
}

// Ve man hinh chinh - goi tab header va tab con
void VeManHinhDocGiaMuonTra(sf::RenderWindow& window, const sf::Font& font) {
    VeHeaderTab(window, font);
    if (tabHienTai == TAB_DOCGIA) VeTabDocGia(window, font);
    else VeTabMuonTra(window, font);
}

// Xu ly su kien: click, nut, input, tab switch
void XuLySuKienDocGiaMuonTra(sf::RenderWindow& window, sf::Event event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        MaUI id = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);

        // Tab header ids 900,901
        if (id == (MaUI)900) { tabHienTai = TAB_DOCGIA; CapNhatThongBaoSFML("Tab: Doc Gia", THONG_TIN); return; }
        if (id == (MaUI)901) { tabHienTai = TAB_MUONTRA; CapNhatThongBaoSFML("Tab: Muon-Tra", THONG_TIN); return; }

        if (tabHienTai == TAB_DOCGIA) {
            switch (id) {
            case NUT_PHAI_DOC_GIA: input_Phai = !input_Phai; break;
            case NUT_TRANG_THAI_DOC_GIA: input_TrangThai = input_TrangThai == 1 ? 0 : 1; break;
            case NUT_THEM_DOC_GIA: {
                std::string kq = themDocGia(rootDocGia_UI, input_Ho, input_Ten, input_Phai, input_TrangThai);
                CapNhatThongBaoSFML(kq, THONG_TIN);
                // refresh root and clear small fields
                rootDocGia_UI = loadDocGia();
                input_Ho.clear(); input_Ten.clear(); input_MaThe.clear();
                break;
            }
            case NUT_HIEU_CHINH_DOC_GIA: {
                if (input_MaThe.empty()) { CapNhatThongBaoSFML("Nhap ma the de hieu chinh", LOI); break; }
                int ma = std::atoi(input_MaThe.c_str());
                std::string kq = capNhatDocGia(rootDocGia_UI, ma, input_Ho, input_Ten, input_Phai, input_TrangThai);
                CapNhatThongBaoSFML(kq, THONG_TIN);
                rootDocGia_UI = loadDocGia();
                break;
            }
            case NUT_XOA_DOC_GIA: {
                if (input_MaThe.empty()) { CapNhatThongBaoSFML("Nhap ma the de xoa", LOI); break; }
                int ma = std::atoi(input_MaThe.c_str());
                // xac nhan xoa - hien thong bao cho nay (UI pop-up khong co -> thong bao va bo chan)
                std::string kq = xoaDocGiaTheoMa(rootDocGia_UI, ma);
                CapNhatThongBaoSFML(kq, THONG_TIN);
                rootDocGia_UI = loadDocGia();
                break;
            }
            case NUT_LIET_KE_DOC_GIA:
                showDanhSachDocGia = !showDanhSachDocGia;
                CapNhatThongBaoSFML(showDanhSachDocGia ? "Hien danh sach" : "An danh sach", THONG_TIN);
                break;
            default: break;
            }
        }
        else { // tab muon-tra
            switch (id) {
            case NUT_MUON_SACH: {
                if (input_MaThe.empty() || input_MaSach.empty()) { CapNhatThongBaoSFML("Nhap ma the va ma sach", LOI); break; }
                int ma = std::atoi(input_MaThe.c_str());
                std::string kq = muonSach(rootDocGia_UI, input_MaSach, ma, layNgayHienTai());
                CapNhatThongBaoSFML(kq, THONG_TIN);
                rootDocGia_UI = loadDocGia();
                break;
            }
            case NUT_TRA_SACH: {
                if (input_MaThe.empty() || input_MaSach.empty()) { CapNhatThongBaoSFML("Nhap ma the va ma sach", LOI); break; }
                int ma = std::atoi(input_MaThe.c_str());
                std::string kq = traSach(rootDocGia_UI, input_MaSach, ma, layNgayHienTai(), chk_MatSach);
                CapNhatThongBaoSFML(kq, THONG_TIN);
                rootDocGia_UI = loadDocGia();
                break;
            }
            case NUT_MAT_SACH: chk_MatSach = !chk_MatSach; break;
            case NUT_LIET_KE_SACH_DANG_MUON:
                showDanhSachMuon = !showDanhSachMuon;
                CapNhatThongBaoSFML(showDanhSachMuon ? "Hien danh sach sach dang muon" : "An danh sach", THONG_TIN);
                break;
            case NUT_LIET_KE_DOC_GIA_QUA_HAN:
                showDanhSachQuaHan = !showDanhSachQuaHan;
                CapNhatThongBaoSFML(showDanhSachQuaHan ? "Hien danh sach qua han" : "An danh sach qua han", THONG_TIN);
                break;
            default: break;
            }
        }
    }

    // Keyboard global escape to menu
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        manHinhHienTai = MENU_CHINH;
    }
}
