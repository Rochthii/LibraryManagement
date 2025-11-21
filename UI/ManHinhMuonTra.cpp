// [FILE: ManHinhMuonTra.cpp]
#include "TrangThaiManHinhMuonTra.h"
#include "GiaoDienSFML.h"
#include "TienIchGiaoDien.h"
#include "DocGia.h"
#include "QuanLySach.h"
#include "Constants.h"
#include "XuLyChuoi.h"

static MuonTraState s;

// Externs
extern PTRDG rootDocGia;
extern PTRDS dsDauSach[];
extern int soLuongDauSach;

// Constants Layout Dock
const float DOCK_L_X = PADDING;
const float DOCK_L_Y = THANH_TAB_CAO + PADDING;
const float DOCK_L_W = CHIEU_RONG * 0.55f; // Dock trái 55%
const float DOCK_L_H = CHIEU_CAO - DOCK_L_Y - PADDING;

const float DOCK_R_X = DOCK_L_X + DOCK_L_W + PADDING;
const float DOCK_R_Y = DOCK_L_Y;
const float DOCK_R_W = CHIEU_RONG - DOCK_R_X - PADDING;
const float DOCK_R_H = DOCK_L_H;

// Helper: Cắt chuỗi hiển thị
static inline std::string CatText(std::string str, size_t len) { // Đổi int -> size_t
    return (str.length() > len) ? str.substr(0, len-3) + "..." : str;
}

// =====================================================
// 1. LOGIC CẬP NHẬT DỮ LIỆU (BACKEND)
// =====================================================

static void TaiDanhSachDocGia() {
    s.soLuongDGHienThi = 0;
    // Lấy toàn bộ độc giả ra mảng (có thể thêm lọc theo tuKhoaTimDG sau này)
    DuyetCayRaMang(rootDocGia, s.mangDocGiaHienThi, s.soLuongDGHienThi);
    
    // Tính phân trang
    s.tongSoTrang = (s.soLuongDGHienThi + SACH_MOI_TRANG - 1) / SACH_MOI_TRANG;
    if (s.tongSoTrang == 0) s.tongSoTrang = 1;
    if (s.trangHienTai > s.tongSoTrang) s.trangHienTai = 1;
}

static void TaiSachDangMuon() {
    if (!s.docGiaDangChon) return;
    s.slSachDangMuon = LayDSSachDangMuon(s.docGiaDangChon, s.listSachDangMuon, 10, dsDauSach, soLuongDauSach);
}

static void TimKiemSachDeMuon() {
    std::string tk = ChuanHoaKhoangTrang(s.tuKhoaTimSach);
    if (tk.empty()) s.soLuongSachTimThay = 0;
    else s.soLuongSachTimThay = timKiemLogic(dsDauSach, soLuongDauSach, tk, s.ketQuaTimSach);
    
    // Reset trang sách
    s.trangHienTai = 1;
    s.tongSoTrang = (s.soLuongSachTimThay + SACH_MOI_TRANG - 1) / SACH_MOI_TRANG;
    if(s.tongSoTrang == 0) s.tongSoTrang = 1;
}

// =====================================================
// 2. VẼ GIAO DIỆN (FRONTEND)
// =====================================================

// --- Màn hình MENU (Home) ---
static void VeMenuMuonTra(sf::RenderWindow &window, const sf::Font &font) {
    (void)window;
    float btnW = 400.f;
    float btnH = 200.f;
    float startX = (CHIEU_RONG - btnW*2 - 50.f)/2;
    float startY = (CHIEU_CAO - btnH)/2;

    TaoNut(font, NUT_MT_VAO_MUON_TRA, startX, startY, btnW, btnH, "MUON / TRA SACH", MAU_NHAN, MAU_CHU_NUT);
    TaoNut(font, NUT_MT_VAO_TOP_10, startX + btnW + 50.f, startY, btnW, btnH, "TOP 10 SACH", sf::Color(255, 100, 100), MAU_CHU_NUT);
}

// --- DOCK TRÁI: Luôn hiển thị danh sách (Độc giả hoặc Sách) ---
static void VeDockTrai(sf::RenderWindow &window, const sf::Font &font) {
    std::string title = "DANH SACH";
    if (s.buocHienTai == BUOC_MUON_SACH) title = "TIM SACH DE MUON";
    else title = "DANH SACH DOC GIA";

    VeKhung(window, DOCK_L_X, DOCK_L_Y, DOCK_L_W, DOCK_L_H, title, font);

    // 1. Ô Tìm Kiếm (Luôn hiện ở đầu Dock trái)
    float currY = DOCK_L_Y + 40.f;
    MaUI idInput = (s.buocHienTai == BUOC_MUON_SACH) ? INPUT_MT_TIM_SACH : INPUT_MT_TIM_DOC_GIA;
    std::string* strVal = (s.buocHienTai == BUOC_MUON_SACH) ? &s.tuKhoaTimSach : &s.tuKhoaTimDG;
    std::string hint = (s.buocHienTai == BUOC_MUON_SACH) ? "Nhap ten sach/ISBN..." : "Nhap ma the/ten doc gia...";

    TaoInput(font, idInput, DOCK_L_X + 10.f, currY, DOCK_L_W - 80.f, 35.f, "", *strVal, hint);
    TaoNut(font, KHONG_XAC_DINH, DOCK_L_X + DOCK_L_W - 60.f, currY, 50.f, 35.f, "TIM", MAU_NEN_NUT, MAU_CHU_NUT);

    // 2. Bảng Danh Sách
    currY += 45.f;
    float rowH = 35.f;
    int startIdx = (s.trangHienTai - 1) * SACH_MOI_TRANG;

    // Header
    sf::RectangleShape header(sf::Vector2f(DOCK_L_W - 20.f, 30.f));
    header.setPosition(DOCK_L_X + 10.f, currY);
    header.setFillColor(MAU_BANG_HEADER);
    window.draw(header);

    sf::Text txtH = TaoVanBan(font, (s.buocHienTai == BUOC_MUON_SACH) ? "ISBN       | TEN SACH" : "MA THE | HO TEN DOC GIA", FONT_SIZE_NHO, MAU_NHAN);
    txtH.setPosition(DOCK_L_X + 20.f, currY + 5.f);
    window.draw(txtH);
    currY += 35.f;

    // Content Rows
    int count = (s.buocHienTai == BUOC_MUON_SACH) ? s.soLuongSachTimThay : s.soLuongDGHienThi;
    int endIdx = std::min(startIdx + SACH_MOI_TRANG, count);

    for (int i = startIdx; i < endIdx; ++i) {
        bool selected = false;
        std::string displayStr = "";
        
        if (s.buocHienTai == BUOC_MUON_SACH) {
            PTRDS ds = s.ketQuaTimSach[i].sach;
            displayStr = ds->ISBN + "   | " + CatText(ds->tenSach, 40);
            if (s.dauSachDangChon == ds) selected = true;
        } else {
            PTRDG dg = s.mangDocGiaHienThi[i];
            displayStr = std::to_string(dg->data.MaThe) + "  | " + CatText(dg->data.Ho + " " + dg->data.Ten, 40);
            if (s.docGiaDangChon == dg) selected = true;
        }

        // Row Background
        if (selected) {
            sf::RectangleShape rowBg(sf::Vector2f(DOCK_L_W - 20.f, rowH));
            rowBg.setPosition(DOCK_L_X + 10.f, currY);
            rowBg.setFillColor(MAU_NHAN); // Highlight
            window.draw(rowBg);
        }

        sf::Text txtR = TaoVanBan(font, displayStr, FONT_SIZE_NHO, selected ? sf::Color::Black : MAU_CHU);
        txtR.setPosition(DOCK_L_X + 20.f, currY + 7.f);
        window.draw(txtR);

        currY += rowH;
    }
    
    // Phân trang
    float pageY = DOCK_L_Y + DOCK_L_H - 40.f;
    TaoNut(font, NUT_MT_PREV_PAGE, DOCK_L_X + 10.f, pageY, 80.f, 30.f, "<<", MAU_NEN_NUT, MAU_CHU_NUT);
    TaoNut(font, NUT_MT_NEXT_PAGE, DOCK_L_X + DOCK_L_W - 90.f, pageY, 80.f, 30.f, ">>", MAU_NEN_NUT, MAU_CHU_NUT);
    sf::Text pg = TaoVanBan(font, std::to_string(s.trangHienTai) + "/" + std::to_string(s.tongSoTrang), FONT_SIZE_NHO, MAU_CHU);
    pg.setPosition(DOCK_L_X + DOCK_L_W/2 - 10.f, pageY + 5.f);
    window.draw(pg);
}

// --- DOCK PHẢI: Hiển thị thông tin & Nút hành động ---
static void VeDockPhai(sf::RenderWindow &window, const sf::Font &font) {
    VeKhung(window, DOCK_R_X, DOCK_R_Y, DOCK_R_W, DOCK_R_H, "THONG TIN & HANH DONG", font);
    float cY = DOCK_R_Y + 50.f;
    float cX = DOCK_R_X + 20.f;

    // TH1: Chưa chọn độc giả
    if (s.buocHienTai == BUOC_CHON_DOC_GIA) {
        sf::Text t = TaoVanBan(font, "Vui long chon DOC GIA\ntu danh sach ben trai\nde thao tac.", FONT_SIZE_BINH_THUONG, MAU_CHU);
        t.setPosition(cX, cY);
        window.draw(t);
        return;
    }

    // Luôn hiện thông tin ĐG đang chọn
    if (s.docGiaDangChon) {
        sf::Text tInfo = TaoVanBan(font, 
            "DOC GIA: " + s.docGiaDangChon->data.Ho + " " + s.docGiaDangChon->data.Ten + "\n" +
            "Ma The: " + std::to_string(s.docGiaDangChon->data.MaThe) + "\n" +
            "Dang muon: " + std::to_string(s.docGiaDangChon->data.soSachDangMuon) + "/3",
            FONT_SIZE_BINH_THUONG, MAU_THANH_CONG);
        tInfo.setPosition(cX, cY);
        window.draw(tInfo);
        
        // Đường kẻ
        sf::RectangleShape line(sf::Vector2f(DOCK_R_W - 40.f, 2.f));
        line.setPosition(cX, cY + 90.f);
        line.setFillColor(MAU_VIEN);
        window.draw(line);
        cY += 110.f;
    }

    // TH2: Menu Hành Động (Xem / Mượn / Trả)
    if (s.buocHienTai == BUOC_CHON_HANH_DONG) {
        float btnH = 50.f;
        TaoNut(font, NUT_MT_HANHDONG_XEM, cX, cY, DOCK_R_W - 40.f, btnH, "1. XEM SACH DANG MUON", MAU_NEN_NUT, MAU_CHU_NUT);
        cY += btnH + 20.f;
        TaoNut(font, NUT_MT_HANHDONG_MUON, cX, cY, DOCK_R_W - 40.f, btnH, "2. MUON SACH MOI", MAU_NHAN, MAU_CHU_NUT);
        cY += btnH + 20.f;
        TaoNut(font, NUT_MT_HANHDONG_TRA, cX, cY, DOCK_R_W - 40.f, btnH, "3. TRA SACH", MAU_LOI, MAU_CHU_NUT);
    }
    
    // TH3: Xem sách đang mượn
    else if (s.buocHienTai == BUOC_XEM_SACH || s.buocHienTai == BUOC_TRA_SACH) {
        sf::Text t = TaoVanBan(font, (s.buocHienTai == BUOC_TRA_SACH) ? "CHON SACH DE TRA:" : "SACH DANG MUON:", FONT_SIZE_BINH_THUONG, MAU_NHAN);
        t.setPosition(cX, cY);
        window.draw(t);
        cY += 30.f;

        if (s.slSachDangMuon == 0) {
            sf::Text empty = TaoVanBan(font, "(Trong)", FONT_SIZE_NHO, MAU_CHU);
            empty.setPosition(cX, cY);
            window.draw(empty);
        } else {
            for (int i=0; i<s.slSachDangMuon; i++) {
                bool selected = (s.maSachCanTra == s.listSachDangMuon[i].maSach);
                std::string row = std::to_string(i+1) + ". " + s.listSachDangMuon[i].maSach + " - " + CatText(s.listSachDangMuon[i].tenSach, 25);
                
                // Nút giả lập dòng
                //MaUI idBtn = (s.buocHienTai == BUOC_TRA_SACH) ? HANG_SACH : KHONG_XAC_DINH; // Chỉ click được khi Trả
                // Logic vẽ custom để xử lý click sau
                sf::Text tR = TaoVanBan(font, row, FONT_SIZE_NHO, selected ? MAU_NHAN : MAU_CHU);
                tR.setPosition(cX, cY);
                window.draw(tR);
                cY += 25.f;
            }
        }

        if (s.buocHienTai == BUOC_TRA_SACH && !s.maSachCanTra.empty()) {
            cY += 20.f;
            TaoNut(font, NUT_MT_XAC_NHAN_TRA, cX, cY, DOCK_R_W - 40.f, 50.f, "XAC NHAN TRA SACH", MAU_LOI, MAU_CHU_NUT);
        }
        
        // Nút quay lại
        TaoNut(font, NUT_MT_HUY_HANH_DONG, cX, DOCK_R_Y + DOCK_R_H - 50.f, 100.f, 30.f, "QUAY LAI", MAU_NEN_NUT, MAU_CHU_NUT);
    }

    // TH4: Đang Mượn Sách
    else if (s.buocHienTai == BUOC_MUON_SACH) {
        sf::Text t = TaoVanBan(font, "SACH DUOC CHON:", FONT_SIZE_BINH_THUONG, MAU_NHAN);
        t.setPosition(cX, cY);
        window.draw(t);
        cY += 30.f;

        if (s.dauSachDangChon) {
            std::string info = s.dauSachDangChon->tenSach + "\n" + 
                               "Tac gia: " + s.dauSachDangChon->tacGia + "\n" + 
                               "ISBN: " + s.dauSachDangChon->ISBN;
            
            // Kiểm tra bản sao
            int avail = DemBanSaoCoTheMuon(s.dauSachDangChon);
            std::string status = "\nCo the muon: " + std::to_string(avail) + " cuon";
            
            sf::Text tS = TaoVanBan(font, info + status, FONT_SIZE_NHO, MAU_CHU);
            tS.setPosition(cX, cY);
            window.draw(tS);
            
            cY += 100.f;
            if (avail > 0) {
                TaoNut(font, NUT_MT_XAC_NHAN_MUON, cX, cY, DOCK_R_W - 40.f, 50.f, "XAC NHAN MUON", MAU_NHAN, MAU_CHU_NUT);
            } else {
                sf::Text err = TaoVanBan(font, "HET SACH!", FONT_SIZE_BINH_THUONG, MAU_LOI);
                err.setPosition(cX, cY);
                window.draw(err);
            }
        } else {
            sf::Text hint = TaoVanBan(font, "Vui long tim va chon\nsach o ben trai.", FONT_SIZE_NHO, sf::Color(150,150,150));
            hint.setPosition(cX, cY);
            window.draw(hint);
        }
        
        TaoNut(font, NUT_MT_HUY_HANH_DONG, cX, DOCK_R_Y + DOCK_R_H - 50.f, 100.f, 30.f, "QUAY LAI", MAU_NEN_NUT, MAU_CHU_NUT);
    }
}

// --- HÀM VẼ CHÍNH ---
void VeManHinhMuonTra(sf::RenderWindow &window, const sf::Font &font) {
    // Header
    sf::RectangleShape top(sf::Vector2f(CHIEU_RONG, THANH_TAB_CAO));
    top.setFillColor(MAU_KHUNG);
    window.draw(top);
    sf::Text t = TaoVanBan(font, "QUAN LY MUON TRA", FONT_SIZE_BINH_THUONG, MAU_TIEU_DE);
    t.setPosition(PADDING, 5.f);
    window.draw(t);
    TaoNut(font, NUT_BACK, CHIEU_RONG - 100.f, 5.f, 90.f, 30.f, "MENU", MAU_NUT_BACK, MAU_CHU_NUT);

    if (s.cheDo == MT_HOME) {
        VeMenuMuonTra(window, font);
    } else if (s.cheDo == MT_PROCESS) {
        VeDockTrai(window, font);
        VeDockPhai(window, font);
    } else if (s.cheDo == MT_TOP_10) {
        // Gọi hàm vẽ Top 10 cũ hoặc viết mới đơn giản
        sf::Text t10 = TaoVanBan(font, "TOP 10 SACH (Dang cap nhat)", 30, MAU_CHU);
        t10.setPosition(100, 100);
        window.draw(t10);
    }

    // Modal
    if (s.hienModal) {
        // Vẽ overlay đen và thông báo (Bạn có thể tái sử dụng code modal cũ)
    }
}

// =====================================================
// 3. XỬ LÝ SỰ KIỆN (EVENT HANDLER)
// =====================================================

void XuLySuKienManHinhMuonTra(sf::RenderWindow &window, sf::Event event) {
    (void)window;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        MaUI id = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);

        // 1. Xử lý Nút Back
        if (id == NUT_BACK) {
            if (s.cheDo == MT_PROCESS && s.buocHienTai != BUOC_CHON_DOC_GIA) {
                // Nếu đang ở bước sâu hơn thì back ra từng cấp
                if (s.buocHienTai == BUOC_CHON_HANH_DONG) {
                    s.docGiaDangChon = nullptr;
                    s.buocHienTai = BUOC_CHON_DOC_GIA;
                } else {
                    s.buocHienTai = BUOC_CHON_HANH_DONG;
                }
            } else if (s.cheDo != MT_HOME) {
                s.cheDo = MT_HOME; // Về menu MuonTra
            } else {
                manHinhHienTai = MENU_CHINH; // Về Menu chính
            }
            return;
        }

        // 2. Xử lý Menu Home
        if (s.cheDo == MT_HOME) {
            if (id == NUT_MT_VAO_MUON_TRA) {
                s.cheDo = MT_PROCESS;
                s.buocHienTai = BUOC_CHON_DOC_GIA;
                TaiDanhSachDocGia(); // Load data
            }
            // else if (NUT_MT_VAO_TOP_10)...
            return;
        }

        // 3. Xử lý trong Dock Trái (Chọn list)
        if (id == HANG_SACH) { // Click vào bảng
            // Tính toán dòng được click dựa trên tọa độ Y
            float listY = DOCK_L_Y + 40.f + 45.f;
            int idx = (event.mouseButton.y - listY) / 35.f; // 35.f là rowHeight
            int realIdx = (s.trangHienTai - 1) * SACH_MOI_TRANG + idx;

            // CLICK CHỌN ĐỘC GIẢ
            if (s.buocHienTai == BUOC_CHON_DOC_GIA) {
                if (realIdx >= 0 && realIdx < s.soLuongDGHienThi) {
                    s.docGiaDangChon = s.mangDocGiaHienThi[realIdx];
                    s.buocHienTai = BUOC_CHON_HANH_DONG; // Chuyển sang chọn hành động ngay
                    TaiSachDangMuon(); // Cache sách mượn
                }
            }
            // CLICK CHỌN SÁCH MƯỢN
            else if (s.buocHienTai == BUOC_MUON_SACH) {
                if (realIdx >= 0 && realIdx < s.soLuongSachTimThay) {
                    s.dauSachDangChon = s.ketQuaTimSach[realIdx].sach;
                }
            }
            
            // CLICK CHỌN SÁCH TRẢ (Ở Dock Phải - Cần check X)
            if (s.buocHienTai == BUOC_TRA_SACH && event.mouseButton.x > DOCK_R_X) {
                 // Logic chọn sách trả tương tự
                 // Cập nhật s.maSachCanTra
            }
        }

        // 4. Xử lý Hành động Dock Phải
        if (id == NUT_MT_HANHDONG_XEM) s.buocHienTai = BUOC_XEM_SACH;
        if (id == NUT_MT_HANHDONG_MUON) {
            s.buocHienTai = BUOC_MUON_SACH;
            s.trangHienTai = 1; // Reset list trái
            // Clear tìm kiếm sách cũ
            s.tuKhoaTimSach = "";
            s.soLuongSachTimThay = 0; 
        }
        if (id == NUT_MT_HANHDONG_TRA) s.buocHienTai = BUOC_TRA_SACH;
        
        if (id == NUT_MT_HUY_HANH_DONG) s.buocHienTai = BUOC_CHON_HANH_DONG;

        // 5. Nút Tìm kiếm
        if (id == NUT_MT_TIM_DG_BTN) TaiDanhSachDocGia(); // Refresh list DG theo keyword (cần code thêm logic lọc)
        if (id == NUT_MT_TIM_SACH_BTN) TimKiemSachDeMuon();
    }

    // Xử lý nhập liệu Text (Tìm kiếm)
    if (event.type == sf::Event::TextEntered) {
         if (inputHoatDong == INPUT_MT_TIM_DOC_GIA) {
             // Cập nhật s.tuKhoaTimDG...
         }
         if (inputHoatDong == INPUT_MT_TIM_SACH) {
             // Cập nhật s.tuKhoaTimSach...
         }
    }
}

void KhoiTaoManHinhMuonTra() {
    s.Reset();
    TaiDanhSachDocGia();
}