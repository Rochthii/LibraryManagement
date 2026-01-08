// [FILE: ManHinhMuonTra.cpp - Phase 1 Implementation]
#include "Constants.h"
#include "DocGia.h"
#include "GiaoDienMuonTra.h"
#include "GiaoDienSFML.h"
#include "KiemTraDuLieu.h"
#include "MuonTra.h"
#include "NgayThang.h"
#include "QuanLySach.h"
#include "TienIchGiaoDien.h"
#include "TrangThaiManHinhMuonTra.h"
#include "XuLyChuoi.h"
#include <algorithm>
#include <iostream>
#include <sstream>

// ===== BIEN TRANG THAI DUY NHAT =====
static MuonTraState state;

// ===== KHAI BAO HAM NOI BO =====
static void VeTabBar(sf::RenderWindow &window, const sf::Font &font,
                     const MuonTraState &s);
static void VeBangDocGia(sf::RenderWindow &window, const sf::Font &font,
                         const MuonTraState &s);
static void VeBangSach(sf::RenderWindow &window, const sf::Font &font,
                       const MuonTraState &s);
static void VeFormMuonTra(sf::RenderWindow &window, const sf::Font &font,
                          const MuonTraState &s);
static void VeFormThemDocGia(sf::RenderWindow &window, const sf::Font &font,
                             const MuonTraState &s);
static void VeKhungThongBao(sf::RenderWindow &window, const sf::Font &font,
                            const MuonTraState &s);

static void ThucHienTimKiemDocGia(PTRDG rootDocGia, MuonTraState &s, bool silent = false);
static void ThucHienXoaDocGia(MuonTraState &s, PTRDG rootDocGia);
static void ThucHienMuonSachSFML(MuonTraState &s);
static void ThucHienTraSachSFML(MuonTraState &s);
static void ThucHienBaoMatSachSFML(MuonTraState &s);
static void CapNhatSachDangMuon(MuonTraState &s);
static void XoaFormNhapLieuSFML(MuonTraState &s);
static void ThucHienTimKiemSach(MuonTraState &s, bool silent = false);
static void CapNhatPhanTrangDocGia(MuonTraState &s);
static void CapNhatPhanTrangSach(MuonTraState &s);
static void XuLyChonDocGia(MuonTraState &s, PTRDG docGia);
static void QuayLaiBangDocGia(MuonTraState &s);

static inline std::string CatChuoi(const std::string &str, size_t maxLen) {
  return (str.length() > maxLen) ? str.substr(0, maxLen - 3) + "..." : str;
}

// ===== HAM VE TAB BAR =====
static void VeTabBar(sf::RenderWindow &window, const sf::Font &font,
                     const MuonTraState &s) {
  (void)window;
  float tabY = BANG_Y; // Vi tri Y cua thanh tab
  float tabH = NUT_CAO;

  // === KHU VUC 1: TREN BANG SACH/DOC GIA (LEFT) ===
  // === KHU VUC 1: TREN BANG SACH/DOC GIA (LEFT) ===
  if (!s.dangHienThiBangSach) {
    float startX = BANG_X;
    float gap = 15.f; // Standard gap

    // 1. Search Bar
    float searchW = 390.f; // Calculated for exact 15px gap with Right Panel
    std::string goiY =
        s.chuoiTimKiemDocGia.empty() ? "Nhap ten hoac ma the..." : "";

    TaoInput(font, INPUT_MT_TIM_DOC_GIA, startX, tabY, searchW, tabH,
             "Tim kiem:", s.chuoiTimKiemDocGia, goiY);
    startX += searchW + 100.f + gap; // Label 100 + gap

    // 2. Nut XOA TIM
    TaoNut(font, NUT_XOA_TIM, startX, tabY, 70.f, tabH, "XOA", MAU_NEN_NUT,
           MAU_CHU_NUT);
    startX += 70.f + gap;

    // Standard width for secondary buttons
    float btnW = 115.f;

    // 3 & 4. Nut Sap Xep (Chi hien khi khong phai che do Qua Han)
    if (s.cheDoHienTai != CHE_DO_QUA_HAN) {
      // 3. Nut Sap Xep Ma
      sf::Color cMa =
          (s.cheDoSapXep == SAP_XEP_THEO_MA_THE) ? MAU_NHAN : MAU_NEN_NUT;
      TaoNut(font, static_cast<MaUI>(NUT_MT_TAB_QUAHAN + 10), startX, tabY,
             btnW, tabH, "SX: MA", cMa, MAU_CHU_NUT);
      startX += btnW + gap;

      // 4. Nut Sap Xep Ten
      sf::Color cTen =
          (s.cheDoSapXep == SAP_XEP_THEO_TEN_HO) ? MAU_NHAN : MAU_NEN_NUT;
      TaoNut(font, static_cast<MaUI>(NUT_MT_TAB_QUAHAN + 11), startX, tabY,
             btnW, tabH, "SX: TEN", cTen, MAU_CHU_NUT);
      startX += btnW + gap;
    }

    // 5. Nut QUA HAN
    sf::Color cQuaHan =
        (s.cheDoHienTai == CHE_DO_QUA_HAN) ? MAU_LOI : MAU_NEN_NUT;
    TaoNut(font, NUT_MT_TAB_QUAHAN, startX, tabY, btnW, tabH, "QUA HAN",
           cQuaHan, MAU_CHU_NUT);
    startX += btnW + gap;

    // 6. Nut TOP 10
    TaoNut(font, NUT_MT_VAO_TOP_10, startX, tabY, btnW, tabH, "TOP 10",
           MAU_NEN_NUT, MAU_CHU_NUT);
  } else {
    // === HEADER KHI DANG XEM SACH ===
    float startX = BANG_X;
    float gap = 15.f;

    // 1. Search Bar Sach
    float searchW = 450.f;
    std::string goiY =
        s.chuoiTimKiemSach.empty() ? "Tim sach theo ten, ISBN, tac gia..." : "";
    TaoInput(font, static_cast<MaUI>(INPUT_MT_TIM_DOC_GIA + 50), startX, tabY,
             searchW, tabH, "Tim sach:", s.chuoiTimKiemSach, goiY);
    startX += searchW + 100.f + gap;

    // 2. Nut XOA TIM
    TaoNut(font, static_cast<MaUI>(NUT_XOA_TIM + 50), startX, tabY, 90.f, tabH,
           "XOA TIM", MAU_NEN_NUT, MAU_CHU_NUT);
    startX += 90.f + 50.f; // Previous weird offset retained/adjusted? Let's fix
                           // it. Actually let's use gap.
    startX = BANG_X + BANG_RONG - 200.f; // Align Back button to right of table

    // 3. Nut Quay Lai (Back to List) - Right Aligned
    TaoNut(font, static_cast<MaUI>(NUT_MT_VAO_MUON_TRA + 102), startX, tabY,
           200.f, tabH, "<< Quay lai DS Doc Gia", MAU_NEN_NUT, MAU_CHU_NUT);
  }

  // === KHU VUC 2: TREN FORM MUON TRA (RIGHT) ===
  // 1. NUT CHUNG (TOGGLE): THEM DOC GIA / MUON TRA
  float fullRightWidth = FORM_RONG;
  sf::Color cToggle = s.dangHienThiFormThemDocGia ? MAU_THANH_CONG : MAU_LOI;
  std::string lblToggle =
      s.dangHienThiFormThemDocGia ? "THEM DOC GIA" : "MUON / TRA";

  TaoNut(font, static_cast<MaUI>(NUT_MT_TAB_QUAHAN + 12), FORM_X, tabY,
         fullRightWidth, tabH, lblToggle, cToggle, MAU_CHU_NUT);
}

// ===== HAM VE BANG DOC GIA =====
static void VeBangDocGia(sf::RenderWindow &window, const sf::Font &font,
                         const MuonTraState &s) {
  // Khong can search bar rieng - da nam tren tab bar roi
  float headerY = BANG_Y + NUT_CAO + 15.f; // Sat duoi tab bar
  float contentY = headerY + 40.f;

  // Align table bottom with the Form's bottom (Top of Notification - spacing)
  float effectiveNotifyY = KHUNG_THONG_BAO_Y + 5.f;
  float tableBottom = effectiveNotifyY - PADDING;

  // Ve khung va header
  std::string tieuDe = (s.cheDoHienTai == CHE_DO_QUA_HAN)
                           ? "DOC GIA QUA HAN (Double-click de chon)"
                           : "DANH SACH DOC GIA (Double-click de chon)";

  VeKhung(window, BANG_X, headerY, BANG_RONG, tableBottom - headerY, tieuDe,
          font);

  // Ve cot header (STT, Ma The, Ho Ten, Phai, Trang Thai, Sach, Qua Han)
  bool isOverdueMode = (s.cheDoHienTai == CHE_DO_QUA_HAN);

  // Dinh nghia cot theo che do
  std::vector<std::string> headers;
  std::vector<float> colWidths;

  if (isOverdueMode) {
    headers = {"STT", "Ma The", "Ho Ten", "Phai", "S.Sach", "So Ngay QH"};
    colWidths = {0.05f, 0.11f, 0.35f, 0.10f, 0.14f, 0.15f};
  } else {
    headers = {"STT", "Ma The", "Ho Ten", "Phai", "Trang Thai", "Sach"};
    colWidths = {0.05f, 0.11f, 0.38f, 0.10f, 0.22f, 0.12f};
  }

  float colX[7];
  colX[0] = BANG_X + PADDING;
  for (size_t i = 0; i < colWidths.size(); ++i) {
    colX[i + 1] = colX[i] + colWidths[i] * (BANG_RONG - PADDING);
  }

  // Header background
  float headerBoxH = 30.f;
  sf::RectangleShape headerBg(
      sf::Vector2f(BANG_RONG - PADDING * 0.8, headerBoxH));
  headerBg.setPosition(BANG_X + PADDING * 0.4, contentY);
  headerBg.setFillColor(MAU_BANG_HEADER);
  window.draw(headerBg);

  // Header text
  sf::Text headerText;
  headerText.setFont(font);
  headerText.setCharacterSize(FONT_SIZE_BINH_THUONG);
  headerText.setFillColor(MAU_NHAN);

  for (size_t i = 0; i < headers.size(); ++i) {
    headerText.setString(headers[i]);
    headerText.setPosition(
        colX[i], contentY + (headerBoxH - headerText.getCharacterSize()) / 2.f);
    window.draw(headerText);
  }

  // Line duoi header
  sf::RectangleShape line(sf::Vector2f(BANG_RONG - 2 * PADDING, 1.f));
  line.setPosition(BANG_X + PADDING, contentY + 30.f);
  line.setFillColor(MAU_BANG_BORDER);
  window.draw(line);

  // Ve data rows
  float currentY = contentY + 35.f;
  float rowH = 30.f;
  int start = (s.trangHienTaiDocGia - 1) * DOC_GIA_MOI_TRANG;
  int end = std::min(start + DOC_GIA_MOI_TRANG, s.soLuongKetQuaDocGia);

  sf::Text dataText;
  dataText.setFont(font);
  dataText.setCharacterSize(FONT_SIZE_BINH_THUONG);

  for (int i = start; i < end; ++i) {
    PTRDG dg = s.ketQuaTimKiemDocGia[i].docGia;
    int overdueDays = s.ketQuaTimKiemDocGia[i].overdueDays;
    if (!dg)
      continue;

    bool isSelected = (dg->data.MaThe == s.maTheDocGiaDuocChon);

    // Highlight row neu duoc chon
    if (isSelected) {
      sf::RectangleShape highlight(
          sf::Vector2f(BANG_RONG - PADDING * 0.8, rowH));
      highlight.setPosition(BANG_X + PADDING * 0.4, currentY - 2.f);
      highlight.setFillColor(MAU_NHAN);
      window.draw(highlight);
    }

    dataText.setFillColor(isSelected ? sf::Color::Black : MAU_CHU);
    float textY = currentY + (rowH - dataText.getCharacterSize()) / 2.f;

    // STT
    dataText.setString(std::to_string(i + 1));
    dataText.setPosition(colX[0], textY);
    window.draw(dataText);

    // Ma The
    dataText.setString(std::to_string(dg->data.MaThe));
    dataText.setPosition(colX[1], textY);
    window.draw(dataText);

    // Ho Ten
    std::string hoTen = dg->data.Ho + " " + dg->data.Ten;
    dataText.setString(CatChuoi(hoTen, isOverdueMode ? 28 : 30));
    dataText.setPosition(colX[2], textY);
    window.draw(dataText);

    // Phai
    dataText.setString(dg->data.Phai == 0 ? "Nam" : "Nu");
    dataText.setPosition(colX[3], textY);
    window.draw(dataText);

    if (isOverdueMode) {
      // So sach dang muon
      dataText.setString(std::to_string(dg->data.soSachDangMuon) + "/3");
      dataText.setPosition(colX[4], textY);
      window.draw(dataText);

      // So ngay qua han
      dataText.setFillColor(isSelected ? sf::Color::Black : MAU_LOI);
      dataText.setStyle(sf::Text::Bold);
      dataText.setString(std::to_string(overdueDays) + " ngay");
      dataText.setPosition(colX[5], textY);
      window.draw(dataText);
      dataText.setStyle(sf::Text::Regular);
      dataText.setFillColor(isSelected ? sf::Color::Black : MAU_CHU);
    } else {
      // Trang Thai
      std::string tt = dg->data.TrangThai ? "Hoat dong" : "Khoa";
      dataText.setFillColor(
          isSelected ? sf::Color::Black
                     : (dg->data.TrangThai ? MAU_THANH_CONG : MAU_LOI));
      dataText.setString(tt);
      dataText.setPosition(colX[4], textY);
      window.draw(dataText);

      // So sach dang muon
      dataText.setFillColor(isSelected ? sf::Color::Black : MAU_CHU);
      dataText.setString(std::to_string(dg->data.soSachDangMuon) + "/3");
      dataText.setPosition(colX[5], textY);
      window.draw(dataText);
    }

    currentY += rowH;
  }

  // ===== NÚT CHỨC NĂNG (Action Buttons) =====
  float actionY = tableBottom + PADDING; // Positioned below table
  float actionH = NUT_CAO;

  if (s.docGiaDangChon) {
    float wBtn = (BANG_RONG - (PADDING * 4)) / 4.f;
    float spacing = PADDING;
    float startX = BANG_X;

    // Chi Tiet
    TaoNut(font, NUT_CHI_TIET_DOC_GIA, startX, actionY, wBtn, actionH,
           "CHI TIET", MAU_NHAN, MAU_CHU_NUT);
    startX += wBtn + spacing;

    // Sua
    TaoNut(font, NUT_SUA_DOC_GIA, startX, actionY, wBtn, actionH, "SUA",
           MAU_NEN_NUT, MAU_CHU_NUT);
    startX += wBtn + spacing;

    // Xoa
    TaoNut(font, NUT_XOA_DOC_GIA, startX, actionY, wBtn, actionH, "XOA",
           MAU_LOI, MAU_CHU_NUT);
    startX += wBtn + spacing;

    // Huy Chon
    TaoNut(font, NUT_HUY_CHON, startX, actionY, wBtn, actionH, "HUY CHON",
           MAU_NEN_NUT, MAU_CHU_NUT);
  }

  // ===== PHAN TRANG (Below Action Buttons) =====
  float pageY = actionY + actionH + PADDING / 2.f;

  TaoNut(font, NUT_MT_PREV_PAGE, BANG_X, pageY, 130.f, NUT_CAO,
         "<< Trang Truoc", MAU_NEN_NUT, MAU_CHU_NUT);
  TaoNut(font, NUT_MT_NEXT_PAGE, BANG_X + 138.f, pageY, 130.f, NUT_CAO,
         "Trang Sau >>", MAU_NEN_NUT, MAU_CHU_NUT);

  std::string pageText = "Trang " + std::to_string(s.trangHienTaiDocGia) +
                         " / " + std::to_string(s.tongSoTrangDocGia);
  sf::Text txtPage = TaoVanBan(font, pageText, FONT_SIZE_BINH_THUONG, MAU_CHU);

  // Align page text vertically center with button
  sf::FloatRect textBounds = txtPage.getLocalBounds();
  txtPage.setPosition(BANG_X + 276.f,
                      pageY + (NUT_CAO - textBounds.height) / 2.f - 5.f);
  window.draw(txtPage);
}

// ===== HAM VE BANG SACH (SAU KHI CHON DOC GIA) =====
static void VeBangSach(sf::RenderWindow &window, const sf::Font &font,
                       const MuonTraState &s) {
  // Can le len tren cung (bang doc gia)
  float headerY = BANG_Y + NUT_CAO + 15.f;
  float contentY = headerY + 40.f;

  // Align table bottom with the Form's bottom (Top of Notification - spacing)
  float effectiveNotifyY = KHUNG_THONG_BAO_Y + 5.f;
  float tableBottom = effectiveNotifyY - PADDING;

  // Removed local search bar / breadcrumb (now in VeTabBar)

  VeKhung(window, BANG_X, headerY, BANG_RONG, tableBottom - headerY,
          "DANH SACH DAU SACH (Double-click de chon)", font);

  // Header columns
  float colWidths[] = {0.05f, 0.16f, 0.30f, 0.07f, 0.18f, 0.07f, 0.11f, 0.06f};
  float colX[9];
  colX[0] = BANG_X + PADDING;
  for (int i = 0; i < 8; ++i) {
    colX[i + 1] = colX[i] + colWidths[i] * (BANG_RONG - PADDING);
  }

  float headerBoxH = 30.f;
  sf::RectangleShape headerBg(
      sf::Vector2f(BANG_RONG - PADDING * 0.8, headerBoxH));
  headerBg.setPosition(BANG_X + PADDING * 0.4, contentY);
  headerBg.setFillColor(MAU_BANG_HEADER);
  window.draw(headerBg);

  std::string headers[] = {"STT",     "ISBN", "Ten Sach", "Trang",
                           "Tac Gia", "NXB",  "The Loai", "SL"};
  sf::Text headerText;
  headerText.setFont(font);
  headerText.setCharacterSize(FONT_SIZE_BINH_THUONG);
  headerText.setFillColor(MAU_NHAN);

  for (int i = 0; i < 8; ++i) {
    headerText.setString(headers[i]);
    headerText.setPosition(
        colX[i], contentY + (headerBoxH - headerText.getCharacterSize()) / 2.f);
    window.draw(headerText);
  }

  sf::RectangleShape line(sf::Vector2f(BANG_RONG - 2 * PADDING, 1.f));
  line.setPosition(BANG_X + PADDING, contentY + 30.f);
  line.setFillColor(MAU_BANG_BORDER);
  window.draw(line);

  // Data rows
  float currentY = contentY + 35.f;
  float rowH = 30.f;
  int start = (s.trangHienTaiSach - 1) * SACH_MOI_TRANG_MUONTRA;
  int end = std::min(start + SACH_MOI_TRANG_MUONTRA, s.soLuongKetQuaSach);

  sf::Text dataText;
  dataText.setFont(font);
  dataText.setCharacterSize(FONT_SIZE_BINH_THUONG);

  sf::RectangleShape highlight(sf::Vector2f(BANG_RONG - PADDING * 0.8, rowH));
  highlight.setFillColor(MAU_NHAN);

  for (int i = start; i < end; ++i) {
    PTRDS d = s.ketQuaTimKiemSach[i].dauSach;
    if (!d)
      continue;

    bool isSelected = (d->ISBN == s.isbnClickCuoi);
    if (isSelected) {
      highlight.setPosition(BANG_X + PADDING * 0.4, currentY - 2.f);
      window.draw(highlight);
    }

    dataText.setFillColor(isSelected ? sf::Color::Black : MAU_CHU);
    float textY = currentY + (rowH - dataText.getCharacterSize()) / 2.f;

    const std::string dataCols[] = {std::to_string(i + 1),
                                    d->ISBN,
                                    CatChuoi(d->tenSach, 25),
                                    std::to_string(d->soTrang),
                                    CatChuoi(d->tacGia, 15),
                                    std::to_string(d->namXuatBan),
                                    CatChuoi(d->theLoai, 8),
                                    std::to_string(DemBanSaoCoTheMuon(d)) +
                                        " / " + std::to_string(d->tongBanSao)};

    for (int col = 0; col < 8; ++col) {
      dataText.setString(dataCols[col]);
      dataText.setPosition(colX[col], textY);
      window.draw(dataText);
    }

    currentY += rowH;
  }

  // Pagination
  float pageY = CHIEU_CAO - PADDING * 4.f - NUT_CAO * 1.8f;
  TaoNut(font, static_cast<MaUI>(NUT_MT_PREV_PAGE + 50), BANG_X + PADDING,
         pageY, 130.f, NUT_CAO, "<< Trang Truoc", MAU_NEN_NUT, MAU_CHU_NUT);
  TaoNut(font, static_cast<MaUI>(NUT_MT_NEXT_PAGE + 50),
         BANG_X + PADDING + 138.f, pageY, 130.f, NUT_CAO, "Trang Sau >>",
         MAU_NEN_NUT, MAU_CHU_NUT);

  std::string pageText = "Trang " + std::to_string(s.trangHienTaiSach) + " / " +
                         std::to_string(s.tongSoTrangSach);
  sf::Text txtPage = TaoVanBan(font, pageText, FONT_SIZE_BINH_THUONG, MAU_CHU);
  txtPage.setPosition(BANG_X + PADDING + 276.f, pageY + 8.f);
  window.draw(txtPage);

  std::string statsText =
      "Tong: " + std::to_string(s.soLuongKetQuaSach) + " sach";
  sf::Text txtStats =
      TaoVanBan(font, statsText, FONT_SIZE_NHO, sf::Color(150, 150, 160));
  txtStats.setPosition(BANG_X + PADDING + 400.f, pageY + 10.f);
  window.draw(txtStats);
}

// ===== HAM VE FORM MUON/TRA =====
static void VeFormMuonTra(sf::RenderWindow &window, const sf::Font &font,
                          const MuonTraState &s) {
  // Fix align with Table Header (BANG_Y + NUT_CAO + 15.f)
  float effectiveFormY = BANG_Y + NUT_CAO + 15.f;
  float effectiveNotifyY = KHUNG_THONG_BAO_Y + 5.f;
  float formH = effectiveNotifyY - effectiveFormY - PADDING;

  VeKhung(window, FORM_X, effectiveFormY, FORM_RONG, formH, "MUON / TRA SACH",
          font);

  float currentY = effectiveFormY + 40.f;
  float labelX = FORM_X + PADDING;
  float contentW = FORM_RONG - 2 * PADDING;

  if (s.docGiaDangChon) {
    PTRDG dg = s.docGiaDangChon;

    // Card thong tin doc gia
    float cardH = 80.f;
    sf::RectangleShape cardBorder(sf::Vector2f(contentW, cardH));
    cardBorder.setPosition(labelX, currentY);
    cardBorder.setFillColor(sf::Color::Transparent);
    cardBorder.setOutlineThickness(1.5f);
    cardBorder.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(cardBorder);

    float cardY = currentY + 10.f;
    sf::Text txtHeader = TaoVanBan(font, "DOC GIA", FONT_SIZE_NHO, MAU_NHAN);
    txtHeader.setStyle(sf::Text::Bold);
    txtHeader.setPosition(labelX + 10.f, cardY);
    window.draw(txtHeader);
    cardY += 18.f;

    std::string line1 = "Ma The: " + std::to_string(dg->data.MaThe) + "  |  " +
                        dg->data.Ho + " " + dg->data.Ten;
    std::string line2 =
        "Phai: " + std::string(dg->data.Phai == 0 ? "Nam" : "Nu") +
        "  |  Trang Thai: " +
        std::string(dg->data.TrangThai == 1 ? "Hoat dong" : "Khoa");

    sf::Text txtLine1 = TaoVanBan(font, line1, FONT_SIZE_NHO, MAU_CHU);
    txtLine1.setPosition(labelX + 10.f, cardY);
    window.draw(txtLine1);

    sf::Text txtLine2 =
        TaoVanBan(font, line2, FONT_SIZE_NHO - 1, sf::Color(100, 100, 110));
    txtLine2.setPosition(labelX + 10.f, cardY + 18.f);
    window.draw(txtLine2);

    currentY += cardH + 15.f;

    // Mini table sach dang muon
    sf::Text txtSachHeader = TaoVanBan(
        font, "SACH DANG MUON (" + std::to_string(s.slSachDangMuon) + "/3)",
        FONT_SIZE_BINH_THUONG, MAU_NHAN);
    txtSachHeader.setPosition(labelX, currentY);
    window.draw(txtSachHeader);
    currentY += 25.f;

    float tableH = 120.f;
    sf::RectangleShape tableBorder(sf::Vector2f(contentW, tableH));
    tableBorder.setPosition(labelX, currentY);
    tableBorder.setFillColor(sf::Color::Transparent);
    tableBorder.setOutlineThickness(1.f);
    tableBorder.setOutlineColor(sf::Color(80, 80, 100));
    window.draw(tableBorder);

    float tableY = currentY + 5.f;
    sf::RectangleShape headerBg(sf::Vector2f(contentW - 2.f, 22.f));
    headerBg.setPosition(labelX + 1.f, tableY);
    sf::Color headerColor = MAU_BANG_HEADER;
    headerColor.a = 100;
    headerBg.setFillColor(headerColor);
    window.draw(headerBg);

    sf::Text colHeader1 = TaoVanBan(font, "Ma Sach", FONT_SIZE_NHO, MAU_NHAN);
    colHeader1.setPosition(labelX + 8.f, tableY + 4.f);
    window.draw(colHeader1);

    sf::Text colHeader2 = TaoVanBan(font, "Ten Sach", FONT_SIZE_NHO, MAU_NHAN);
    colHeader2.setPosition(labelX + contentW * 0.35f, tableY + 4.f);
    window.draw(colHeader2);

    sf::Text colHeader3 = TaoVanBan(font, "Ngay Muon", FONT_SIZE_NHO, MAU_NHAN);
    colHeader3.setPosition(labelX + contentW * 0.75f, tableY + 4.f);
    window.draw(colHeader3);

    tableY += 28.f;

    if (s.slSachDangMuon > 0) {
      for (int i = 0; i < s.slSachDangMuon && i < 3; ++i) {
        const ThongTinSachDangMuon_DTO &dto = s.listSachDangMuon[i];
        bool quaHan = (dto.soNgayGiu > 7);
        bool sapQuaHan = (dto.soNgayGiu >= 5 && dto.soNgayGiu <= 7);
        bool isSelected = (dto.maSach == s.maSachDangChon);

        sf::Color bgColor = sf::Color::Transparent;
        if (isSelected) {
          bgColor = sf::Color(255, 250, 205, 180);
        } else if (quaHan) {
          bgColor = sf::Color(255, 100, 100, 80);
        } else if (sapQuaHan) {
          bgColor = sf::Color(255, 200, 100, 60);
        }

        if (bgColor != sf::Color::Transparent) {
          sf::RectangleShape rowBg(sf::Vector2f(contentW - 2.f, 22.f));
          rowBg.setPosition(labelX + 1.f, tableY);
          rowBg.setFillColor(bgColor);
          window.draw(rowBg);
        }

        // Store hitbox
        sf::RectangleShape rowHitbox(sf::Vector2f(contentW - 2.f, 22.f));
        rowHitbox.setPosition(labelX + 1.f, tableY);
        rowHitbox.setFillColor(sf::Color::Transparent);
        UIElement elem;
        elem.hinhDang = rowHitbox;
        elem.id = static_cast<MaUI>(HANG_SACH + 1000 + i);
        if (soLuongElement < SO_ELEMENT_TOI_DA) {
          cacElement[soLuongElement++] = elem;
        }

        sf::Color textColor =
            quaHan ? sf::Color(220, 50, 50)
                   : (sapQuaHan ? sf::Color(255, 150, 0) : MAU_CHU);

        sf::Text cell1 = TaoVanBan(font, CatChuoi(dto.maSach, 15),
                                   FONT_SIZE_NHO - 1, textColor);
        cell1.setPosition(labelX + 8.f, tableY + 4.f);
        window.draw(cell1);

        sf::Text cell2 = TaoVanBan(font, CatChuoi(dto.tenSach, 22),
                                   FONT_SIZE_NHO - 1, textColor);
        cell2.setPosition(labelX + contentW * 0.35f, tableY + 4.f);
        window.draw(cell2);

        std::string ngayText = dto.ngayMuon;
        if (quaHan) {
          ngayText += " (!" + std::to_string(dto.soNgayGiu) + " ngay)";
        } else if (sapQuaHan) {
          ngayText += " (" + std::to_string(dto.soNgayGiu) + " ngay)";
        }
        sf::Text cell3 =
            TaoVanBan(font, ngayText, FONT_SIZE_NHO - 1, textColor);
        cell3.setPosition(labelX + contentW * 0.75f, tableY + 4.f);
        window.draw(cell3);

        tableY += 24.f;
      }
    } else {
      sf::Text txtEmpty = TaoVanBan(font, "(Chua muon sach nao)", FONT_SIZE_NHO,
                                    sf::Color(150, 150, 160));
      txtEmpty.setPosition(labelX + contentW / 2.f - 70.f, tableY + 20.f);
      window.draw(txtEmpty);
    }

    currentY += tableH + 15.f;

    // Input ma sach
    float inputW = contentW * 0.7f;
    std::string goiY = s.chuoiMaSach.empty() ? "VD: 9780321563842-1" : "";
    TaoInput(font, INPUT_MT_TIM_SACH, labelX, currentY, inputW, INPUT_CAO,
             "Ma Sach (*):", s.chuoiMaSach, goiY);

    if (s.slSachDangMuon > 0 && s.maSachDangChon.empty()) {
      sf::Text txtHint = TaoVanBan(font, "<-- Click sach", FONT_SIZE_NHO - 2,
                                   sf::Color(150, 150, 160));
      txtHint.setPosition(labelX + inputW + 10.f,
                          currentY + INPUT_CAO / 2.f - 6.f);
      window.draw(txtHint);
    }

    currentY += INPUT_CAO + 12.f;

    // Smart buttons
    float btnW = contentW;
    float btnH = NUT_CAO + 5.f;

    bool daDienMaSach = !s.chuoiMaSach.empty();
    bool daChonSachDangMuon = !s.maSachDangChon.empty();
    bool coTheMuon = (s.slSachDangMuon < 3);
    bool docGiaBiKhoa = (dg->data.TrangThai == 0);

    if (daChonSachDangMuon && daDienMaSach) {
      sf::Color mauTra =
          docGiaBiKhoa ? sf::Color(100, 100, 100) : MAU_THANH_CONG;
      sf::Color mauBaoMat = docGiaBiKhoa ? sf::Color(100, 100, 100) : MAU_LOI;

      TaoNut(font, NUT_MT_XAC_NHAN_TRA, labelX, currentY, btnW, btnH + 3.f,
             "TRA SACH", mauTra, MAU_CHU_NUT);
      currentY += btnH + 11.f;

      TaoNut(font, NUT_MT_BAO_MAT, labelX, currentY, btnW, btnH, "BAO MAT SACH",
             mauBaoMat, MAU_CHU_NUT);
      currentY += btnH + 10.f;

      std::string helpMsg =
          docGiaBiKhoa ? "! Doc gia bi khoa. Khong the thao tac."
                       : "Da chon sach dang muon. Chon thao tac ben tren.";
      sf::Color helpColor =
          docGiaBiKhoa ? sf::Color(255, 150, 0) : sf::Color(100, 200, 100);
      sf::Text txtHelp = TaoVanBan(font, helpMsg, FONT_SIZE_NHO - 2, helpColor);
      txtHelp.setPosition(labelX, currentY);
      window.draw(txtHelp);
    } else {
      bool coSachDeTra = (s.slSachDangMuon > 0);
      sf::Color mauMuon = (coTheMuon && !docGiaBiKhoa)
                              ? MAU_THANH_CONG
                              : sf::Color(100, 100, 100);
      sf::Color mauTra =
          (coSachDeTra && !docGiaBiKhoa) ? MAU_NHAN : sf::Color(100, 100, 100);
      sf::Color mauBaoMat =
          (coSachDeTra && !docGiaBiKhoa) ? MAU_LOI : sf::Color(100, 100, 100);

      TaoNut(font, NUT_MT_XAC_NHAN_MUON, labelX, currentY, btnW, btnH,
             "MUON SACH", mauMuon, MAU_CHU_NUT);
      currentY += btnH + 8.f;

      TaoNut(font, NUT_MT_XAC_NHAN_TRA, labelX, currentY, btnW, btnH,
             "TRA SACH", mauTra, MAU_CHU_NUT);
      currentY += btnH + 8.f;

      TaoNut(font, NUT_MT_BAO_MAT, labelX, currentY, btnW, btnH, "BAO MAT SACH",
             mauBaoMat, MAU_CHU_NUT);
      currentY += btnH + 10.f;

      std::string helpText;
      sf::Color helpColor;

      if (docGiaBiKhoa) {
        helpText = "! Doc gia bi khoa. Khong the muon/tra sach.";
        helpColor = sf::Color(220, 100, 80);
      } else if (!coTheMuon) {
        helpText = "! Doc gia da muon du 3 sach. Tra sach truoc khi muon tiep.";
        helpColor = sf::Color(220, 100, 80);
      } else if (s.slSachDangMuon > 0) {
        helpText = "Click vao sach trong bang de chon, hoac nhap ma sach moi.";
        helpColor = sf::Color(120, 120, 140);
      } else {
        helpText = "-> Double click vao sach de muon.";
        helpColor = sf::Color(120, 120, 140);
      }

      sf::Text txtHelp =
          TaoVanBan(font, helpText, FONT_SIZE_NHO - 2, helpColor);
      txtHelp.setPosition(labelX, currentY);
      window.draw(txtHelp);
    }
  } else {
    // Chua chon doc gia
    float emptyCardH = 100.f;
    sf::RectangleShape emptyCard(sf::Vector2f(contentW, emptyCardH));
    emptyCard.setPosition(labelX, currentY + 40.f);
    emptyCard.setFillColor(sf::Color::Transparent);
    emptyCard.setOutlineThickness(1.5f);
    emptyCard.setOutlineColor(sf::Color(100, 100, 120));
    window.draw(emptyCard);

    sf::Text txtIcon = TaoVanBan(font, "?", 48, sf::Color(180, 180, 200));
    txtIcon.setPosition(labelX + contentW / 2.f - 15.f, currentY + 50.f);
    window.draw(txtIcon);

    sf::Text txtEmpty1 =
        TaoVanBan(font, "Chua chon doc gia", FONT_SIZE_BINH_THUONG,
                  sf::Color(120, 120, 140));
    txtEmpty1.setPosition(labelX + contentW / 2.f - 75.f, currentY + 100.f);
    window.draw(txtEmpty1);

    sf::Text txtEmpty2 =
        TaoVanBan(font, "Double-click vao dong trong bang ben trai de chon",
                  FONT_SIZE_NHO, sf::Color(150, 150, 170));
    txtEmpty2.setPosition(labelX + 30.f, currentY + 122.f);
    window.draw(txtEmpty2);
  }
}

// ===== HAM VE FORM THEM DOC GIA =====
static void VeFormThemDocGia(sf::RenderWindow &window, const sf::Font &font,
                             const MuonTraState &s) {
  float effectiveFormY = BANG_Y + NUT_CAO + 15.f;
  float effectiveNotifyY = KHUNG_THONG_BAO_Y + 5.f;
  float formHeight = effectiveNotifyY - effectiveFormY - PADDING;

  std::string formTitle = s.dangSuaDocGia ? "SUA THONG TIN" : "THEM DOC GIA";
  VeKhung(window, FORM_X, effectiveFormY, FORM_RONG, formHeight, formTitle,
          font);

  float currentY = effectiveFormY + 40.f;
  float labelX = FORM_X + PADDING;
  float inputSpacing = PADDING / 2.f;

  // Hint động
  std::string goiYHo = s.chuoiHo.empty()
                           ? "Toi da " + std::to_string(MAX_TAC_GIA) + " ky tu"
                           : std::to_string(s.chuoiHo.length()) + "/" +
                                 std::to_string(MAX_TAC_GIA);
  std::string goiYTen = s.chuoiTen.empty()
                            ? "Toi da " + std::to_string(MAX_TAC_GIA) + " ky tu"
                            : std::to_string(s.chuoiTen.length()) + "/" +
                                  std::to_string(MAX_TAC_GIA);

  // Input Họ
  TaoInput(font, INPUT_HO_DOC_GIA, labelX, currentY, INPUT_RONG, INPUT_CAO,
           "Ho (*):", s.chuoiHo, goiYHo);
  currentY += INPUT_CAO + inputSpacing;

  // Input Tên
  TaoInput(font, INPUT_TEN_DOC_GIA, labelX, currentY, INPUT_RONG, INPUT_CAO,
           "Ten (*):", s.chuoiTen, goiYTen);
  currentY += INPUT_CAO + inputSpacing;

  // ===== RADIO BUTTON: PHÁI =====
  sf::Text lblPhai =
      TaoVanBan(font, "Phai (*):", FONT_SIZE_BINH_THUONG, MAU_NHAN);
  lblPhai.setPosition(labelX, currentY);
  window.draw(lblPhai);

  float radioX = labelX + 130.f;
  float radioSize = 15.f;

  // Radio Nam
  sf::CircleShape radioNam(radioSize / 2.f);
  radioNam.setPosition(radioX, currentY + 5.f);
  radioNam.setFillColor(sf::Color::Transparent);
  radioNam.setOutlineThickness(2.f);
  radioNam.setOutlineColor(MAU_VIEN);
  window.draw(radioNam);

  if (s.phaiDuocChon == 0) { // Nam được chọn
    sf::CircleShape innerNam(radioSize / 4.f);
    innerNam.setPosition(radioX + radioSize / 4.f,
                         currentY + 5.f + radioSize / 4.f);
    innerNam.setFillColor(MAU_NHAN);
    window.draw(innerNam);
  }

  sf::Text txtNam = TaoVanBan(font, "Nam", FONT_SIZE_BINH_THUONG, MAU_CHU);
  txtNam.setPosition(radioX + radioSize + 5.f, currentY);
  window.draw(txtNam);

  // Radio Nữ
  radioX += 120.f;
  sf::CircleShape radioNu(radioSize / 2.f);
  radioNu.setPosition(radioX, currentY + 5.f);
  radioNu.setFillColor(sf::Color::Transparent);
  radioNu.setOutlineThickness(2.f);
  radioNu.setOutlineColor(MAU_VIEN);
  window.draw(radioNu);

  if (s.phaiDuocChon == 1) { // Nữ được chọn
    sf::CircleShape innerNu(radioSize / 4.f);
    innerNu.setPosition(radioX + radioSize / 4.f,
                        currentY + 5.f + radioSize / 4.f);
    innerNu.setFillColor(MAU_NHAN);
    window.draw(innerNu);
  }

  sf::Text txtNu = TaoVanBan(font, "Nu", FONT_SIZE_BINH_THUONG, MAU_CHU);
  txtNu.setPosition(radioX + radioSize + 5.f, currentY);
  window.draw(txtNu);

  currentY += INPUT_CAO + inputSpacing;

  // ===== RADIO BUTTON: TRẠNG THÁI =====
  sf::Text lblTrangThai =
      TaoVanBan(font, "Trang Thai (*):", FONT_SIZE_BINH_THUONG, MAU_NHAN);
  lblTrangThai.setPosition(labelX, currentY);
  window.draw(lblTrangThai);

  radioX = labelX + 130.f;

  // Radio Hoạt động
  sf::CircleShape radioHD(radioSize / 2.f);
  radioHD.setPosition(radioX, currentY + 5.f);
  radioHD.setFillColor(sf::Color::Transparent);
  radioHD.setOutlineThickness(2.f);
  radioHD.setOutlineColor(MAU_VIEN);
  window.draw(radioHD);

  if (s.trangThaiDuocChon == 1) {
    sf::CircleShape innerHD(radioSize / 4.f);
    innerHD.setPosition(radioX + radioSize / 4.f,
                        currentY + 5.f + radioSize / 4.f);
    innerHD.setFillColor(MAU_THANH_CONG);
    window.draw(innerHD);
  }

  sf::Text txtHD = TaoVanBan(font, "Hoat dong", FONT_SIZE_BINH_THUONG, MAU_CHU);
  txtHD.setPosition(radioX + radioSize + 5.f, currentY);
  window.draw(txtHD);

  // Radio Khóa
  radioX += 160.f;
  sf::CircleShape radioKhoa(radioSize / 2.f);
  radioKhoa.setPosition(radioX, currentY + 5.f);
  radioKhoa.setFillColor(sf::Color::Transparent);
  radioKhoa.setOutlineThickness(2.f);
  radioKhoa.setOutlineColor(MAU_VIEN);
  window.draw(radioKhoa);

  if (s.trangThaiDuocChon == 0) {
    sf::CircleShape innerKhoa(radioSize / 4.f);
    innerKhoa.setPosition(radioX + radioSize / 4.f,
                          currentY + 5.f + radioSize / 4.f);
    innerKhoa.setFillColor(MAU_LOI);
    window.draw(innerKhoa);
  }

  sf::Text txtKhoa = TaoVanBan(font, "Khoa", FONT_SIZE_BINH_THUONG, MAU_CHU);
  txtKhoa.setPosition(radioX + radioSize + 5.f, currentY);
  window.draw(txtKhoa);

  currentY += INPUT_CAO + PADDING * 1.5f;

  // ===== NÚT THEM / LUU =====
  float totalWidth = FORM_RONG - 2 * PADDING;
  float nutRongDon = 250.f;
  float nutXDon = labelX + (totalWidth - nutRongDon) / 2;
  float nutCao = NUT_CAO + 10.f;

  std::string btnLabel = s.dangSuaDocGia ? "LUU THAY DOI" : "THEM DOC GIA";
  sf::Color btnColor = s.dangSuaDocGia ? MAU_NHAN : MAU_THANH_CONG;

  TaoNut(font, NUT_THEM_DOC_GIA, nutXDon, currentY, nutRongDon, nutCao,
         btnLabel, btnColor, MAU_CHU_NUT);
}

// ===== HAM VE KHUNG THONG BAO =====
static void VeKhungThongBao(sf::RenderWindow &window, const sf::Font &font,
                            const MuonTraState &s) {
  float effectiveNotifyY = KHUNG_THONG_BAO_Y + 5.f;
  VeKhung(window, FORM_X, effectiveNotifyY, FORM_RONG, KHUNG_THONG_BAO_CAO,
          "THONG BAO", font);

  if (s.xacNhanXoaDocGia && s.docGiaDangChon) {
    // VIẾT CHẾ ĐỘ XÁC NHẬN XOÁ (Tham khảo ManHinhQuanLyDocGia.cpp)
    std::string confirmMsg = "Ban co chac muon xoa Doc Gia " +
                             std::to_string(s.docGiaDangChon->data.MaThe) +
                             "?\n(\"" + s.docGiaDangChon->data.Ho + " " +
                             s.docGiaDangChon->data.Ten + "\")";

    float maxW = FORM_RONG - 2 * PADDING - 10.f;
    std::string wrapped =
        WordWrapText(font, confirmMsg, FONT_SIZE_BINH_THUONG, maxW);

    sf::Text txtMsg = TaoVanBan(font, wrapped, FONT_SIZE_BINH_THUONG, MAU_LOI);
    txtMsg.setPosition(FORM_X + PADDING, effectiveNotifyY + 35.f);
    window.draw(txtMsg);

    // Nut Xac nhan / Huy
    float buttonY = effectiveNotifyY + 85.f;
    float nutRong = (FORM_RONG - 3 * PADDING) / 2.f;

    TaoNut(font, NUT_XAC_NHAN_XOA_DG, FORM_X + PADDING, buttonY, nutRong,
           NUT_CAO, "XAC NHAN XOA", MAU_LOI, MAU_CHU_NUT);
    TaoNut(font, NUT_HUY_XOA_DG, FORM_X + PADDING + nutRong + PADDING, buttonY,
           nutRong, NUT_CAO, "HUY", MAU_NEN_NUT, MAU_CHU_NUT);
  } else {
    // Hiển thị thông báo bình thường
    if (noiDungThongBao.empty())
      return;

    sf::Color mauChu = (loaiThongBao == 1)   ? MAU_LOI
                       : (loaiThongBao == 2) ? MAU_THANH_CONG
                                             : MAU_CHU;
    float maxW = FORM_RONG - 2 * PADDING - 10.f;
    std::string wrapped =
        WordWrapText(font, noiDungThongBao, FONT_SIZE_BINH_THUONG, maxW);

    sf::Text txt = TaoVanBan(font, wrapped, FONT_SIZE_BINH_THUONG, mauChu);
    txt.setPosition(FORM_X + PADDING, effectiveNotifyY + 35.f);
    window.draw(txt);
  }
}

// ===== HAM VE MODAL CHI TIET DOC GIA =====
static void VeModalChiTietDocGia(sf::RenderWindow &window, const sf::Font &font,
                                 const MuonTraState &s) {
  if (!s.docGiaDangChon)
    return;

  // Overlay
  sf::RectangleShape overlay(sf::Vector2f(CHIEU_RONG, CHIEU_CAO));
  overlay.setFillColor(sf::Color(0, 0, 0, 150));
  window.draw(overlay);

  float modalRong = 700.f;
  float modalCao = 400.f;
  float modalX = (CHIEU_RONG - modalRong) / 2.f;
  float modalY = (CHIEU_CAO - modalCao) / 2.f;
  float padding = 20.f;

  std::string tenDG =
      s.docGiaDangChon->data.Ho + " " + s.docGiaDangChon->data.Ten;
  VeKhung(window, modalX, modalY, modalRong, modalCao,
          "SACH DANG MUON - " + tenDG, font);

  // Hien thi so luong
  sf::Text infoText =
      TaoVanBan(font, "Tong: " + std::to_string(s.slSachDangMuon) + "/3 cuon",
                FONT_SIZE_NHO, MAU_NHAN);
  infoText.setPosition(modalX + modalRong - 180.f, modalY + 35.f);
  window.draw(infoText);

  // Header
  float currentY = modalY + 60.f;
  float col1X = modalX + padding;
  float col2X = col1X + 50.f;
  float col3X = col2X + 200.f;
  float col4X = col3X + 250.f;

  sf::Text header = TaoVanBan(font, "STT", FONT_SIZE_BINH_THUONG, MAU_NHAN);
  header.setPosition(col1X, currentY);
  window.draw(header);
  header.setString("Ma Sach");
  header.setPosition(col2X, currentY);
  window.draw(header);
  header.setString("Ten Sach");
  header.setPosition(col3X, currentY);
  window.draw(header);
  header.setString("Ngay Muon");
  header.setPosition(col4X, currentY);
  window.draw(header);

  sf::RectangleShape line(sf::Vector2f(modalRong - 2 * padding, 1.f));
  line.setPosition(col1X, currentY + 30.f);
  line.setFillColor(sf::Color(100, 100, 100));
  window.draw(line);

  currentY += 35.f;

  // Data rows
  sf::Text dataText = TaoVanBan(font, "", FONT_SIZE_NHO, MAU_CHU);
  for (int i = 0; i < s.slSachDangMuon; ++i) {
    const auto &book = s.listSachDangMuon[i];

    dataText.setString(std::to_string(i + 1));
    dataText.setPosition(col1X, currentY);
    window.draw(dataText);

    dataText.setString(book.maSach);
    dataText.setPosition(col2X, currentY);
    window.draw(dataText);

    dataText.setString(CatChuoi(book.tenSach, 25));
    dataText.setPosition(col3X, currentY);
    window.draw(dataText);

    dataText.setString(book.ngayMuon);
    dataText.setPosition(col4X, currentY);
    window.draw(dataText);

    currentY += 25.f;
  }

  // Nut dong (Su dung NUT_DONG_MODAL_DG cho dong bo)
  float btnY = modalY + modalCao - 60.f;
  TaoNut(font, NUT_DONG_MODAL_DG, modalX + modalRong - 130.f - padding, btnY,
         130.f, 40.f, "DONG", MAU_LOI, MAU_CHU_NUT);

  // [FIX] Ve nut luon vi TaoNut khong ve khi goi sau vong lap chinh
  if (soLuongElement > 0) {
    window.draw(cacElement[soLuongElement - 1].hinhDang);
    window.draw(cacElement[soLuongElement - 1].vanBan);
  }
}

// ===== HAM TAI DU LIEU TOP 10 =====
static void TaiDuLieuTop10(MuonTraState &s) {
  extern PTRDS dsDauSach[];
  extern int soLuongDauSach;

  TopSachDTO bufferTam[MAX_DAUSACH];
  int soLuong = LayTopSach(dsDauSach, soLuongDauSach, bufferTam);

  s.soLuongTop = soLuong;
  for (int i = 0; i < soLuong && i < 10; ++i) {
    s.top10[i] = bufferTam[i];
  }

  if (s.soLuongTop > 0) {
    CapNhatThongBaoSFML("Da tai du lieu Top 10 sach.", 0);
  } else {
    CapNhatThongBaoSFML("Chua co du lieu muon sach.", 1);
  }
}

// ===== HAM VE MODAL TOP 10 =====
static void VeModalTop10(sf::RenderWindow &window, const sf::Font &font,
                         const MuonTraState &s) {
  // Overlay
  sf::RectangleShape overlay(sf::Vector2f(CHIEU_RONG, CHIEU_CAO));
  overlay.setFillColor(sf::Color(0, 0, 0, 180));
  window.draw(overlay);

  // Modal box
  float modalW = CHIEU_RONG * 0.85f;
  float modalH = CHIEU_CAO * 0.80f;
  float modalX = (CHIEU_RONG - modalW) / 2.f;
  float modalY = (CHIEU_CAO - modalH) / 2.f;

  VeKhung(window, modalX, modalY, modalW, modalH,
          "TOP 10 SACH DUOC MUON NHIEU NHAT", font);

  float currentY = modalY + 50.f;
  float labelX = modalX + PADDING;

  // Table header
  std::string headers[] = {"#",       "ISBN",     "Ten Sach",
                           "Tac Gia", "The Loai", "Luot Muon"};
  float colWidths[] = {0.05f, 0.15f, 0.30f, 0.20f, 0.15f, 0.15f};
  float colX[7];
  colX[0] = labelX;
  for (int i = 0; i < 6; ++i) {
    colX[i + 1] = colX[i] + colWidths[i] * (modalW - 2 * PADDING);
  }

  sf::Text headerText;
  headerText.setFont(font);
  headerText.setCharacterSize(FONT_SIZE_BINH_THUONG);
  headerText.setFillColor(MAU_NHAN);

  for (int i = 0; i < 6; ++i) {
    headerText.setString(headers[i]);
    headerText.setPosition(colX[i], currentY);
    window.draw(headerText);
  }

  // Line under header
  sf::RectangleShape line(sf::Vector2f(modalW - 2 * PADDING, 1.f));
  line.setPosition(labelX, currentY + 30.f);
  line.setFillColor(sf::Color(100, 100, 100));
  window.draw(line);

  currentY += 35.f;

  // Draw rows
  for (int i = 0; i < s.soLuongTop && i < 10; ++i) {
    const TopSachDTO &dto = s.top10[i];
    if (!dto.dauSach)
      continue;

    // Highlight top 3
    sf::Color bgColor = sf::Color::Transparent;
    std::string medalIcon = "";
    if (i == 0) {
      bgColor = sf::Color(255, 215, 0, 100); // Gold
      medalIcon = "1";
    } else if (i == 1) {
      bgColor = sf::Color(192, 192, 192, 100); // Silver
      medalIcon = "2";
    } else if (i == 2) {
      bgColor = sf::Color(205, 127, 50, 100); // Bronze
      medalIcon = "3";
    }

    if (bgColor != sf::Color::Transparent) {
      sf::RectangleShape rowBg(sf::Vector2f(modalW - 2 * PADDING, 28.f));
      rowBg.setPosition(labelX, currentY);
      rowBg.setFillColor(bgColor);
      window.draw(rowBg);
    }

    sf::Text cellText;
    cellText.setFont(font);
    cellText.setCharacterSize(FONT_SIZE_NHO);
    cellText.setFillColor(MAU_CHU);

    // Rank
    std::string rankText = medalIcon.empty()
                               ? std::to_string(i + 1)
                               : medalIcon + " #" + std::to_string(i + 1);
    cellText.setString(rankText);
    cellText.setPosition(colX[0], currentY + 5.f);
    if (!medalIcon.empty()) {
      cellText.setStyle(sf::Text::Bold);
      cellText.setFillColor(MAU_NHAN);
    }
    window.draw(cellText);
    cellText.setStyle(sf::Text::Regular);
    cellText.setFillColor(MAU_CHU);

    cellText.setString(dto.dauSach->ISBN);
    cellText.setPosition(colX[1], currentY + 5.f);
    window.draw(cellText);

    cellText.setString(CatChuoi(dto.dauSach->tenSach, 30));
    cellText.setPosition(colX[2], currentY + 5.f);
    window.draw(cellText);

    cellText.setString(CatChuoi(dto.dauSach->tacGia, 20));
    cellText.setPosition(colX[3], currentY + 5.f);
    window.draw(cellText);

    cellText.setString(CatChuoi(dto.dauSach->theLoai, 15));
    cellText.setPosition(colX[4], currentY + 5.f);
    window.draw(cellText);

    cellText.setString(std::to_string(dto.dauSach->soLuotMuon));
    cellText.setPosition(colX[5], currentY + 5.f);
    window.draw(cellText);

    currentY += 28.f;
  }

  // Close button (Reuse button ID to toggle)
  float btnY = modalY + modalH - 60.f;
  TaoNut(font, static_cast<MaUI>(NUT_MT_VAO_TOP_10 + 1),
         modalX + modalW - 140.f, btnY, 120.f, 40.f, "DONG", MAU_LOI,
         MAU_CHU_NUT);

  // [FIX] Ve nut luon vi TaoNut khong ve khi goi sau vong lap chinh
  if (soLuongElement > 0) {
    window.draw(cacElement[soLuongElement - 1].hinhDang);
    window.draw(cacElement[soLuongElement - 1].vanBan);
  }
}

// ===== HAM VE CHINH =====
void VeManHinhMuonTra(sf::RenderWindow &window, const sf::Font &font) {
  // Thanh tieu de
  sf::RectangleShape topBar(sf::Vector2f(CHIEU_RONG, THANH_TAB_CAO));
  topBar.setFillColor(MAU_KHUNG);
  window.draw(topBar);

  sf::Text title = TaoVanBan(font, "QUAN LY MUON / TRA & DOC GIA",
                             FONT_SIZE_BINH_THUONG, MAU_TIEU_DE);
  title.setPosition(PADDING,
                    PADDING / 2.f - title.getLocalBounds().height / 2.f);
  window.draw(title);

  // Nut quay ve menu
  TaoNut(font, NUT_BACK, CHIEU_RONG - PADDING - 100.f, PADDING / 4.f, 100.f,
         NUT_CAO * 0.8f, "< MENU", MAU_NUT_BACK, MAU_CHU_NUT);

  // Ve tab bar
  VeTabBar(window, font, state);

  // Hien thi dua tren che do
  if (state.cheDoHienTai == CHE_DO_MUON_TRA ||
      state.cheDoHienTai == CHE_DO_QUA_HAN) {
    if (state.dangHienThiBangSach) {
      VeBangSach(window, font, state);
    } else {
      VeBangDocGia(window, font, state);
    }

    // Hien form dua tren toggle
    if (state.dangHienThiFormThemDocGia) {
      VeFormThemDocGia(window, font, state);
    } else {
      VeFormMuonTra(window, font, state);
    }
  }

  VeKhungThongBao(window, font, state);

  VeKhungThongBao(window, font, state);

  // XOA: Modal drawing moved to VeModalMuonTra to fix Z-Index
}

// [NEW] Ham ve cac modal muon tra (Goi tu GiaoDienSFML.cpp sau cung)
void VeModalMuonTra(sf::RenderWindow &window, const sf::Font &font) {
  if (state.hienThiModalTop10) {
    VeModalTop10(window, font, state);
  }
  if (state.hienThiModalChiTiet) {
    VeModalChiTietDocGia(window, font, state);
  }
}

// ===== HAM XU LY SU KIEN =====
void XuLySuKienManHinhMuonTra(sf::RenderWindow &window, sf::Event event, PTRDG rootDocGia) {
  (void)window;

  // Xu ly modal Chi tiet doc gia
  if (state.hienThiModalChiTiet) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
      MaUI element =
          LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);
      if (element == NUT_DONG_MODAL_DG) {
        state.hienThiModalChiTiet = false;
      }
    }
    return;
  }

  // Xu ly modal TOP 10
  if (state.hienThiModalTop10) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
      MaUI element =
          LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);
      if (element == static_cast<MaUI>(NUT_MT_VAO_TOP_10 + 1)) {
        state.hienThiModalTop10 = false;
      }
    }
    return; // Khong xu ly gi khac khi modal dang mo
  }

  if (event.type == sf::Event::MouseButtonPressed &&
      event.mouseButton.button == sf::Mouse::Left) {
    MaUI element = LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);

    // Xu ly nut BACK
    if (element == NUT_BACK) {
      manHinhHienTai = MENU_CHINH;
      state.Reset();
      inputHoatDong = KHONG_XAC_DINH;
      return;
    }

    // Xu ly chuyen tab

    // Xu ly NUT TOP 10
    if (element == NUT_MT_VAO_TOP_10) {
      TaiDuLieuTop10(state);
      state.hienThiModalTop10 = true;
      return;
    }
    // Xu ly nut QUA HAN tab
    if (element == NUT_MT_TAB_QUAHAN) {
      if (state.cheDoHienTai == CHE_DO_QUA_HAN) {
        // Toggle OFF -> Ve che do Muon Tra
        state.cheDoHienTai = CHE_DO_MUON_TRA;
        CapNhatThongBaoSFML("Che do: Muon/Tra sach", 0);
      } else {
        // Toggle ON
        state.cheDoHienTai = CHE_DO_QUA_HAN;
        CapNhatThongBaoSFML("Che do: Doc gia qua han", 0);
      }
      state.dangHienThiBangSach = false;
      state.dangHienThiFormThemDocGia = false;
      ThucHienTimKiemDocGia(rootDocGia, state);
      return;
    }

    // Xu ly nut sap xep "Theo Ma The"
    if (element == static_cast<MaUI>(NUT_MT_TAB_QUAHAN + 10)) {
      state.cheDoSapXep = SAP_XEP_THEO_MA_THE;
      ThucHienTimKiemDocGia(rootDocGia, state); // Reload data with new sorting
      CapNhatThongBaoSFML("Sap xep: Theo ma the", 0);
      return;
    }

    // Xu ly nut sap xep "Theo Ten+Ho"
    if (element == static_cast<MaUI>(NUT_MT_TAB_QUAHAN + 11)) {
      state.cheDoSapXep = SAP_XEP_THEO_TEN_HO;
      ThucHienTimKiemDocGia(rootDocGia, state); // Reload data with new sorting
      CapNhatThongBaoSFML("Sap xep: Theo ten+ho", 0);
      return;
    }

    // Xu ly nut toggle "THEM DOC GIA / MUON TRA" (Nut moi hop nhat)
    if (element == static_cast<MaUI>(NUT_MT_TAB_QUAHAN + 12)) {
      state.dangHienThiFormThemDocGia = !state.dangHienThiFormThemDocGia;

      if (state.dangHienThiFormThemDocGia) {
        CapNhatThongBaoSFML("Che do: QUAN LY DOC GIA (Them/Sua/Xoa)", 0);
      } else {
        state.dangSuaDocGia = false; // Reset editing state when going back
        CapNhatThongBaoSFML("Che do: MUON / TRA SACH", 0);
      }
      return;
    }

    // Xu ly nut TOP 10
    if (element == NUT_MT_VAO_TOP_10) {
      state.hienThiModalTop10 = true;
      // TODO: Load top 10 data
      CapNhatThongBaoSFML("Hien thi Top 10 sach duoc muon nhieu nhat", 0);
      return;
    }

    // Xu ly nut quay lai danh sach doc gia (tu bang sach)
    if (element == static_cast<MaUI>(NUT_MT_VAO_MUON_TRA + 102)) {
      QuayLaiBangDocGia(state);
      return;
    }

    // Xu ly input tim doc gia
    if (element == INPUT_MT_TIM_DOC_GIA) {
      inputHoatDong = INPUT_MT_TIM_DOC_GIA;
      return;
    }

    // Xu ly input tim sach (khi da chon doc gia)
    if (element == static_cast<MaUI>(INPUT_MT_TIM_DOC_GIA + 50)) {
      inputHoatDong = static_cast<MaUI>(INPUT_MT_TIM_DOC_GIA + 50);
      return;
    }

    // Xu ly input ma sach
    if (element == INPUT_MT_TIM_SACH) {
      inputHoatDong = INPUT_MT_TIM_SACH;
      return;
    }

    // Xu ly nut XOA TIM doc gia
    if (element == NUT_XOA_TIM) {
      state.chuoiTimKiemDocGia = "";
      inputHoatDong = KHONG_XAC_DINH;
      ThucHienTimKiemDocGia(rootDocGia, state);
      return;
    }

    // Xu ly nut XOA TIM sach
    if (element == static_cast<MaUI>(NUT_XOA_TIM + 50)) {
      state.chuoiTimKiemSach = "";
      inputHoatDong = KHONG_XAC_DINH;
      ThucHienTimKiemSach(state);
      return;
    }

    // Xu ly Action Buttons (Chi Tiet, Sua, Xoa, Huy Chon)
    if (state.docGiaDangChon) {
      if (element == NUT_CHI_TIET_DOC_GIA) {
        extern PTRDS dsDauSach[];
        extern int soLuongDauSach;
        state.slSachDangMuon =
            LayDSSachDangMuon(state.docGiaDangChon, state.listSachDangMuon, 10,
                              dsDauSach, soLuongDauSach);
        state.hienThiModalChiTiet = true;
        return;
      }

      if (element == NUT_SUA_DOC_GIA) {
        state.dangHienThiFormThemDocGia = true;
        state.dangSuaDocGia = true;
        state.chuoiHo = state.docGiaDangChon->data.Ho;
        state.chuoiTen = state.docGiaDangChon->data.Ten;
        state.phaiDuocChon = state.docGiaDangChon->data.Phai;
        state.trangThaiDuocChon = state.docGiaDangChon->data.TrangThai;
        CapNhatThongBaoSFML(
            "Da chuyen sang che do SUA. Thay doi thong tin ben phai.", 0);
        return;
      }

      if (element == NUT_XOA_DOC_GIA) {
        if (state.docGiaDangChon->data.soSachDangMuon > 0) {
          CapNhatThongBaoSFML(
              "Khong the xoa! Doc gia dang muon " +
                  std::to_string(state.docGiaDangChon->data.soSachDangMuon) +
                  " cuon.",
              1);
        } else {
          state.xacNhanXoaDocGia = true;
          CapNhatThongBaoSFML("", 0);
        }
        return;
      }

      if (element == NUT_HUY_CHON || element == NUT_HUY_CHON_DOC_GIA) {
        state.docGiaDangChon = nullptr;
        state.maTheDocGiaDuocChon = 0;
        state.dangSuaDocGia = false;
        state.chuoiHo = "";
        state.chuoiTen = "";
        state.xacNhanXoaDocGia = false;
        CapNhatThongBaoSFML("Da huy chon doc gia.", 0);
        return;
      }
    }

    if (element == NUT_XAC_NHAN_XOA_DG) {
      ThucHienXoaDocGia(state, rootDocGia);
      return;
    }

    if (element == NUT_HUY_XOA_DG) {
      state.xacNhanXoaDocGia = false;
      CapNhatThongBaoSFML("Da huy xoa.", 0);
      return;
    }

    // Xu ly pagination doc gia
    if (element == NUT_MT_PREV_PAGE) {
      if (state.trangHienTaiDocGia > 1)
        state.trangHienTaiDocGia--;
      return;
    }
    if (element == NUT_MT_NEXT_PAGE) {
      if (state.trangHienTaiDocGia < state.tongSoTrangDocGia)
        state.trangHienTaiDocGia++;
      return;
    }

    // Xu ly pagination sach
    if (element == static_cast<MaUI>(NUT_MT_PREV_PAGE + 50)) {
      if (state.trangHienTaiSach > 1)
        state.trangHienTaiSach--;
      return;
    }
    if (element == static_cast<MaUI>(NUT_MT_NEXT_PAGE + 50)) {
      if (state.trangHienTaiSach < state.tongSoTrangSach)
        state.trangHienTaiSach++;
      return;
    }

    // Xu ly click vao hang sach dang muon trong form
    if (element >= static_cast<MaUI>(HANG_SACH + 1000) &&
        element < static_cast<MaUI>(HANG_SACH + 1010)) {
      int index = element - static_cast<MaUI>(HANG_SACH + 1000);
      if (index < state.slSachDangMuon) {
        state.maSachDangChon = state.listSachDangMuon[index].maSach;
        state.chuoiMaSach = state.maSachDangChon;
        inputHoatDong = KHONG_XAC_DINH;
      }
      return;
    }

    // Xu ly click vao hang doc gia (double-click detection)
    if (element == HANG_SACH && !state.dangHienThiBangSach) {
      // Lay vi tri click
      int mouseY = event.mouseButton.y;

      // === FIX OFFSET ===
      // Phai khop hoan toan voi ham VeBangDocGia
      float headerY = BANG_Y + NUT_CAO + 15.f; // Sat duoi tab bar
      float contentY = headerY + 40.f + 35.f;  // header(40) + line_padding(35)

      float rowHeight = 30.f;
      int rowIndex = static_cast<int>((mouseY - contentY) / rowHeight);

      // Fix: Neu click hoi lech len tren van chap nhan la row 0
      if (rowIndex == -1 && (mouseY - contentY) > -10.f)
        rowIndex = 0;

      int start = (state.trangHienTaiDocGia - 1) * DOC_GIA_MOI_TRANG;
      int end = std::min(start + DOC_GIA_MOI_TRANG, state.soLuongKetQuaDocGia);
      int actualIndex = start + rowIndex;

      if (actualIndex >= 0 && actualIndex < end &&
          actualIndex < state.soLuongKetQuaDocGia) {
        PTRDG clickedDocGia = state.ketQuaTimKiemDocGia[actualIndex].docGia;
        if (clickedDocGia) {
          int maThe = clickedDocGia->data.MaThe;

          // Check double-click
          float elapsedTime =
              state.doubleClickClock.getElapsedTime().asSeconds();
          if (state.maTheClickCuoi == maThe &&
              elapsedTime < state.THOI_GIAN_DOUBLE_CLICK) {
            // DOUBLE CLICK DETECTED
            if (state.dangHienThiFormThemDocGia) {
              // Neu dang o mode them doc gia -> Select row & show action
              // buttons (KHONG chuyen sang sach)
              state.docGiaDangChon = clickedDocGia;
              state.maTheDocGiaDuocChon = maThe;
              CapNhatThongBaoSFML("Da chon doc gia: " + clickedDocGia->data.Ho +
                                      " " + clickedDocGia->data.Ten,
                                  0);
            } else {
              // Mode Muon/Tra -> Chuyen sang xem sach
              XuLyChonDocGia(state, clickedDocGia);
            }
          } else {
            // Single click
            state.maTheDocGiaDuocChon = maThe;

            if (state.dangHienThiFormThemDocGia) {
              // Also set docGiaDangChon for styling/buttons
              state.docGiaDangChon = clickedDocGia;
            }
          }

          state.maTheClickCuoi = maThe;
          state.doubleClickClock.restart();
        }
      }
      return;
    }

    // Xu ly click vao hang sach (double-click detection khi dang hien bang
    // sach)
    if (element == HANG_SACH && state.dangHienThiBangSach) {
      // Lay vi tri click
      int mouseY = event.mouseButton.y;
      float headerY = BANG_Y + NUT_CAO + 15.f;
      float contentY = headerY + 40.f + 35.f;

      float rowHeight = 30.f;
      int rowIndex = static_cast<int>((mouseY - contentY) / rowHeight);

      int start = (state.trangHienTaiSach - 1) * SACH_MOI_TRANG_MUONTRA;
      int end =
          std::min(start + SACH_MOI_TRANG_MUONTRA, state.soLuongKetQuaSach);
      int actualIndex = start + rowIndex;

      if (actualIndex >= 0 && actualIndex < end &&
          actualIndex < state.soLuongKetQuaSach) {
        PTRDS clickedSach = state.ketQuaTimKiemSach[actualIndex].dauSach;
        if (clickedSach) {
          std::string isbn = clickedSach->ISBN;

          // Check double-click
          float elapsedTime =
              state.doubleClickClock.getElapsedTime().asSeconds();
          if (state.isbnClickCuoi == isbn &&
              elapsedTime < state.THOI_GIAN_DOUBLE_CLICK) {
            // DOUBLE CLICK - Chon sach de muon
            // DOUBLE CLICK - Chon sach de muon
            std::string maChon = TimMaSachCoTheMuon(isbn);

            if (!maChon.empty()) {
              state.chuoiMaSach = maChon;
              inputHoatDong = INPUT_MT_TIM_SACH;
              CapNhatThongBaoSFML("Da tu dong chon ban sao: " + maChon, 2);
            } else {
              state.chuoiMaSach = isbn + "-";
              inputHoatDong = INPUT_MT_TIM_SACH;
              CapNhatThongBaoSFML(
                  "Loi: Dau sach nay khong con ban sao nao de muon!", 1);
            }
          } else {
            // Single click - Highlight
            state.isbnClickCuoi = isbn;
          }

          state.doubleClickClock.restart();
        }
      }
      return;
    }

    // Xu ly cac nut muon/tra/bao mat
    if (element == NUT_MT_XAC_NHAN_MUON) {
      ThucHienMuonSachSFML(state);
      return;
    }

    if (element == NUT_MT_XAC_NHAN_TRA) {
      ThucHienTraSachSFML(state);
      return;
    }

    if (element == NUT_MT_BAO_MAT) {
      ThucHienBaoMatSachSFML(state);
      return;
    }

    // Xu ly form Them Doc Gia
    if (element == INPUT_HO_DOC_GIA) {
      inputHoatDong = INPUT_HO_DOC_GIA;
      return;
    }

    if (element == INPUT_TEN_DOC_GIA) {
      inputHoatDong = INPUT_TEN_DOC_GIA;
      return;
    }

    if (element == NUT_THEM_DOC_GIA) {
      // [LOGIC THEM / SUA DOC GIA]
      extern bool duLieuDaThayDoi;

      // 1. Pre-process & Standardize
      std::string hoRaw = state.chuoiHo;
      std::string tenRaw = state.chuoiTen;

      // Call backend validation (which handles length and ASCII checks)
      std::string loi = KiemTraDuLieuDocGia(hoRaw, tenRaw);
      if (!loi.empty()) {
        CapNhatThongBaoSFML(loi, 1);
        return;
      }

      // 2. Final Normalization for Saving
      std::string ho = ChuyenThanhTitleCase(ChuanHoaKhoangTrang(hoRaw));
      std::string ten = ChuyenThanhTitleCase(ChuanHoaKhoangTrang(tenRaw));

      // Update UI state with normalized strings for visual feedback
      state.chuoiHo = ho;
      state.chuoiTen = ten;

      if (state.dangSuaDocGia && state.docGiaDangChon) {
        // UPDATE
        state.docGiaDangChon->data.Ho = ho;
        state.docGiaDangChon->data.Ten = ten;
        state.docGiaDangChon->data.Phai = state.phaiDuocChon;
        state.docGiaDangChon->data.TrangThai = state.trangThaiDuocChon;

        duLieuDaThayDoi = true;
        CapNhatThongBaoSFML(
            "Cap nhat thanh cong doc gia " +
                std::to_string(state.docGiaDangChon->data.MaThe),
            2);

        // Refresh list silently but keep selection
        ThucHienTimKiemDocGia(rootDocGia, state, true);

        // [AUTO HIGHLIGHT] Find page and select
        int maThe = state.docGiaDangChon->data.MaThe;
        for (int i = 0; i < state.soLuongKetQuaDocGia; ++i) {
          if (state.ketQuaTimKiemDocGia[i].docGia->data.MaThe == maThe) {
            state.trangHienTaiDocGia = (i / DOC_GIA_MOI_TRANG) + 1;
            break;
          }
        }
        state.dangHienThiBangSach = false; // Show readers list
      } else {
        // ADD NEW
        PTRDG dg = taoDocGia(ho, ten, state.phaiDuocChon != 0,
                             state.trangThaiDuocChon, rootDocGia);
        if (dg == nullptr || dg->data.MaThe == -1) {
          CapNhatThongBaoSFML("Loi: Khong the sinh ma the (Full)!", 1);
          return;
        }

        InsertDocGia(rootDocGia, dg);
        duLieuDaThayDoi = true;

        // Refresh list
        ThucHienTimKiemDocGia(rootDocGia, state);

        // [AUTO HIGHLIGHT] Find page and select new reader
        int maThe = dg->data.MaThe;
        for (int i = 0; i < state.soLuongKetQuaDocGia; ++i) {
          if (state.ketQuaTimKiemDocGia[i].docGia->data.MaThe == maThe) {
            state.trangHienTaiDocGia = (i / DOC_GIA_MOI_TRANG) + 1;
            break;
          }
        }

        state.maTheDocGiaDuocChon = maThe;
        state.docGiaDangChon = dg;
        state.dangHienThiBangSach = false; // Show readers list

        CapNhatThongBaoSFML(
            "Them thanh cong Doc Gia: " + std::to_string(dg->data.MaThe), 2);
      }

      // Reset info inputs but NOT selection if in edit mode
      if (!state.dangSuaDocGia) {
        state.chuoiHo = "";
        state.chuoiTen = "";
      }
      return;
    }

    // Click vao radio button Phai (Nam/Nu)
    int mouseX = event.mouseButton.x;
    int mouseY = event.mouseButton.y;

    if (state.dangHienThiFormThemDocGia) {
      float currentY = FORM_Y + 40.f + INPUT_CAO * 2 + PADDING;
      float radioX = FORM_X + PADDING + 130.f;
      float radioSize = 15.f;

      // Check click Nam
      if (mouseX >= radioX && mouseX <= radioX + radioSize + 50.f &&
          mouseY >= currentY && mouseY <= currentY + 30.f) {
        state.phaiDuocChon = 0;
        return;
      }

      // Check click Nu
      radioX += 120.f;
      if (mouseX >= radioX && mouseX <= radioX + radioSize + 50.f &&
          mouseY >= currentY && mouseY <= currentY + 30.f) {
        state.phaiDuocChon = 1;
        return;
      }

      // Check click Trang Thai (Hoat dong/Khoa)
      currentY += INPUT_CAO + PADDING / 2.f;
      radioX = FORM_X + PADDING + 130.f;

      // Check click Hoat dong
      if (mouseX >= radioX && mouseX <= radioX + radioSize + 100.f &&
          mouseY >= currentY && mouseY <= currentY + 30.f) {
        state.trangThaiDuocChon = 1;
        return;
      }

      // Check click Khoa
      radioX += 160.f;
      if (mouseX >= radioX && mouseX <= radioX + radioSize + 50.f &&
          mouseY >= currentY && mouseY <= currentY + 30.f) {
        state.trangThaiDuocChon = 0;
        return;
      }
    }

    // Action handling moved to consolidated block above
  }

  // Xu ly text input
  if (event.type == sf::Event::TextEntered ||
      (event.type == sf::Event::KeyPressed &&
       event.key.code == sf::Keyboard::Backspace)) {

    // Input tim doc gia
    if (inputHoatDong == INPUT_MT_TIM_DOC_GIA) {
      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::Backspace) {
        if (!state.chuoiTimKiemDocGia.empty()) {
          state.chuoiTimKiemDocGia.pop_back();
          ThucHienTimKiemDocGia(rootDocGia, state);
        }
      } else if (event.type == sf::Event::TextEntered &&
                 event.text.unicode < 128 && event.text.unicode != 8 &&
                 event.text.unicode != 13) {
        char c = static_cast<char>(event.text.unicode);
        if (state.chuoiTimKiemDocGia.length() < 100) {
          state.chuoiTimKiemDocGia += c;
          ThucHienTimKiemDocGia(rootDocGia, state);
        }
      }
    }

    // Input tim sach
    if (inputHoatDong == static_cast<MaUI>(INPUT_MT_TIM_DOC_GIA + 50)) {
      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::Backspace) {
        if (!state.chuoiTimKiemSach.empty()) {
          state.chuoiTimKiemSach.pop_back();
          ThucHienTimKiemSach(state);
        }
      } else if (event.type == sf::Event::TextEntered &&
                 event.text.unicode < 128 && event.text.unicode != 8 &&
                 event.text.unicode != 13) {
        char c = static_cast<char>(event.text.unicode);
        if (state.chuoiTimKiemSach.length() < 100) {
          state.chuoiTimKiemSach += c;
          ThucHienTimKiemSach(state);
        }
      }
    }

    if (inputHoatDong == INPUT_MT_TIM_SACH) {
      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::Backspace) {
        if (!state.chuoiMaSach.empty()) {
          state.chuoiMaSach.pop_back();
          state.maSachDangChon = ""; // Clear selection
        }
      } else if (event.type == sf::Event::TextEntered &&
                 event.text.unicode < 128 && event.text.unicode != 8 &&
                 event.text.unicode != 13) {
        char c = static_cast<char>(event.text.unicode);
        // Only allow 0-9 and -
        if ((c >= '0' && c <= '9') || c == '-') {
          if (state.chuoiMaSach.length() < 50) {
            state.chuoiMaSach += c;
            state.maSachDangChon = ""; // Clear selection when typing
          }
        } else if (c >= 32) {
          CapNhatThongBaoSFML(
              "Loi: Ma sach chi duoc nhap so (0-9) va dau gach ngang (-).", 1);
        }
      }
    }

    if (inputHoatDong == INPUT_HO_DOC_GIA) {
      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::Backspace) {
        if (!state.chuoiHo.empty()) {
          state.chuoiHo.pop_back();
        }
      } else if (event.type == sf::Event::TextEntered &&
                 event.text.unicode < 128 && event.text.unicode != 8 &&
                 event.text.unicode != 13) {
        char c = static_cast<char>(event.text.unicode);
        // Only allow A-Z, a-z and space
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ') {
          if (state.chuoiHo.length() < 30) {
            state.chuoiHo += c;
          }
        } else if (c >= 32) {
          CapNhatThongBaoSFML("Loi: Ho chi duoc nhap chu cai khong dau (A-Z)!",
                              1);
        }
      }
    }

    if (inputHoatDong == INPUT_TEN_DOC_GIA) {
      if (event.type == sf::Event::KeyPressed &&
          event.key.code == sf::Keyboard::Backspace) {
        if (!state.chuoiTen.empty()) {
          state.chuoiTen.pop_back();
        }
      } else if (event.type == sf::Event::TextEntered &&
                 event.text.unicode < 128 && event.text.unicode != 8 &&
                 event.text.unicode != 13) {
        char c = static_cast<char>(event.text.unicode);
        // Only allow A-Z, a-z and space
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' ') {
          if (state.chuoiTen.length() < 10) {
            state.chuoiTen += c;
          }
        } else if (c >= 32) {
          CapNhatThongBaoSFML("Loi: Ten chi duoc nhap chu cai khong dau (A-Z)!",
                              1);
        }
      }
    }
  }
}

static void CapNhatSachDangMuon(MuonTraState &s) {
  s.slSachDangMuon =
      LayDSSachDangMuonBackend(s.docGiaDangChon, s.listSachDangMuon);
}

static void XoaFormNhapLieuSFML(MuonTraState &s) {
  s.chuoiMaSach = "";
  s.maSachDangChon = "";
  s.indexSachDangChon = -1;
}

static void ThucHienMuonSachSFML(MuonTraState &s) {
  if (!s.docGiaDangChon) {
    CapNhatThongBaoSFML("Loi: Chua chon doc gia!", 1);
    return;
  }

  // Kiểm tra đã mượn đủ 3 sách chưa (UI constraint)
  if (s.slSachDangMuon >= 3) {
    CapNhatThongBaoSFML("Loi: Doc gia da muon du 3 sach!\nVui long tra sach "
                        "truoc khi muon tiep.",
                        1);
    return;
  }

  std::string maSach = CatKhoangTrang(s.chuoiMaSach);
  if (maSach.empty()) {
    CapNhatThongBaoSFML("Loi: Chua nhap ma sach!", 1);
    inputHoatDong = INPUT_MT_TIM_SACH;
    return;
  }

  // Call backend
  std::string result = ThucHienMuonSachBackend(s.docGiaDangChon, maSach);

  if (result.find("Loi:") == std::string::npos) {
    CapNhatThongBaoSFML("Muon sach thanh cong: " + result, 2);
    CapNhatSachDangMuon(s);
    XoaFormNhapLieuSFML(s);
    inputHoatDong = KHONG_XAC_DINH;
  } else {
    CapNhatThongBaoSFML(result, 1);
    inputHoatDong = INPUT_MT_TIM_SACH;
  }
}

static void ThucHienTraSachSFML(MuonTraState &s) {
  if (!s.docGiaDangChon) {
    CapNhatThongBaoSFML("Loi: Chua chon doc gia!", 1);
    return;
  }

  if (s.maSachDangChon.empty()) {
    CapNhatThongBaoSFML(
        "Loi: Chua chon sach de tra!\nClick vao dong sach trong "
        "danh sach de chon.",
        1);
    return;
  }

  std::string loi = ThucHienTraSachBackend(s.docGiaDangChon, s.maSachDangChon);

  if (loi.empty()) {
    CapNhatThongBaoSFML("Tra sach thanh cong: " + s.maSachDangChon, 2);
    CapNhatSachDangMuon(s);
    XoaFormNhapLieuSFML(s);
  } else {
    CapNhatThongBaoSFML(loi, 1);
  }
}

static void ThucHienBaoMatSachSFML(MuonTraState &s) {
  if (!s.docGiaDangChon) {
    CapNhatThongBaoSFML("Loi: Chua chon doc gia!", 1);
    return;
  }

  if (s.maSachDangChon.empty()) {
    CapNhatThongBaoSFML("Loi: Chua chon sach de bao mat!\nClick vao dong sach "
                        "trong danh sach de chon.",
                        1);
    return;
  }

  std::string loi = ThucHienMatSachBackend(s.docGiaDangChon, s.maSachDangChon);

  if (loi.empty()) {
    CapNhatThongBaoSFML("Bao mat sach thanh cong: " + s.maSachDangChon, 2);
    CapNhatSachDangMuon(s);
    XoaFormNhapLieuSFML(s);
  } else {
    CapNhatThongBaoSFML(loi, 1);
  }
}

static void ThucHienXoaDocGia(MuonTraState &s, PTRDG rootDocGia) {
  extern bool duLieuDaThayDoi;

  if (s.docGiaDangChon == nullptr)
    return;

  int maThe = s.docGiaDangChon->data.MaThe;
  std::string hoTen =
      s.docGiaDangChon->data.Ho + " " + s.docGiaDangChon->data.Ten;

  // Xóa khỏi cây AVL
  xoaDocGia(rootDocGia, maThe);
  duLieuDaThayDoi = true;

  CapNhatThongBaoSFML("Xoa thanh cong Doc Gia: " + std::to_string(maThe) +
                          " (" + hoTen + ")",
                      2);

  // Reset state
  s.xacNhanXoaDocGia = false;
  s.docGiaDangChon = nullptr;
  s.maTheDocGiaDuocChon = 0;
  s.dangSuaDocGia = false;

  // Tải lại danh sách (đừng silent để xem còn bao nhiêu)
  ThucHienTimKiemDocGia(rootDocGia, s);
}

// ===== HAM LOGIC =====
static void ThucHienTimKiemDocGia(PTRDG rootDocGia, MuonTraState &s, bool silent) {
  bool laCheDoQuaHan = (s.cheDoHienTai == CHE_DO_QUA_HAN);
  bool sapXepTheoTen = (s.cheDoSapXep == SAP_XEP_THEO_TEN_HO);

  LayDanhSachDocGiaBackend(rootDocGia, s.chuoiTimKiemDocGia, laCheDoQuaHan, sapXepTheoTen, s.ketQuaTimKiemDocGia, s.soLuongKetQuaDocGia);

  // --- UI LOGIC ---
  s.trangHienTaiDocGia = 1;
  CapNhatPhanTrangDocGia(s);

  if (!silent) {
    if (s.soLuongKetQuaDocGia > 0) {
      if (laCheDoQuaHan) {
        CapNhatThongBaoSFML("Tim thay " +
                                std::to_string(s.soLuongKetQuaDocGia) +
                                " doc gia qua han.",
                            0);
      } else {
        CapNhatThongBaoSFML(
            "Tim thay " + std::to_string(s.soLuongKetQuaDocGia) + " doc gia.",
            0);
      }
    } else {
      CapNhatThongBaoSFML("Khong tim thay doc gia nao.", 1);
    }
  }
}

static void CapNhatPhanTrangDocGia(MuonTraState &s) {
  s.tongSoTrangDocGia =
      (s.soLuongKetQuaDocGia + DOC_GIA_MOI_TRANG - 1) / DOC_GIA_MOI_TRANG;
  if (s.tongSoTrangDocGia == 0)
    s.tongSoTrangDocGia = 1;
  if (s.trangHienTaiDocGia > s.tongSoTrangDocGia)
    s.trangHienTaiDocGia = s.tongSoTrangDocGia;
}

static void ThucHienTimKiemSach(MuonTraState &s, bool silent) {
  LayDanhSachSachBackend(s.chuoiTimKiemSach, s.ketQuaTimKiemSach,
                         s.soLuongKetQuaSach);

  s.trangHienTaiSach = 1;
  CapNhatPhanTrangSach(s);

  if (!silent) {
    if (s.soLuongKetQuaSach > 0) {
      CapNhatThongBaoSFML(
          "Tim thay " + std::to_string(s.soLuongKetQuaSach) + " sach.", 0);
    } else {
      CapNhatThongBaoSFML("Khong tim thay sach nao.", 1);
    }
  }
}

static void CapNhatPhanTrangSach(MuonTraState &s) {
  s.tongSoTrangSach = (s.soLuongKetQuaSach + SACH_MOI_TRANG_MUONTRA - 1) /
                      SACH_MOI_TRANG_MUONTRA;
  if (s.tongSoTrangSach == 0)
    s.tongSoTrangSach = 1;
  if (s.trangHienTaiSach > s.tongSoTrangSach)
    s.trangHienTaiSach = s.tongSoTrangSach;
}

static void XuLyChonDocGia(MuonTraState &s, PTRDG docGia) {
  if (!docGia)
    return;

  s.docGiaDangChon = docGia;
  s.maTheDocGiaDuocChon = docGia->data.MaThe;
  s.dangHienThiBangSach = true;

  // Lay danh sach sach dang muon tu backend
  CapNhatSachDangMuon(s);

  // Tim kiem sach (hien tat ca ban dau)
  s.chuoiTimKiemSach = "";
  ThucHienTimKiemSach(s);

  std::string hoTen = docGia->data.Ho + " " + docGia->data.Ten;
  CapNhatThongBaoSFML(
      "Da chon: " + hoTen + " (" + std::to_string(docGia->data.MaThe) + ")", 0);
}

static void QuayLaiBangDocGia(MuonTraState &s) {
  s.dangHienThiBangSach = false;
  s.docGiaDangChon = nullptr;
  s.maTheDocGiaDuocChon = 0;
  s.maSachDangChon = "";
  s.chuoiMaSach = "";
  s.chuoiTimKiemSach = "";
  s.slSachDangMuon = 0;

  CapNhatThongBaoSFML("Quay lai danh sach doc gia.", 0);
}

// ===== HAM KHOI TAO =====
void KhoiTaoManHinhMuonTra(PTRDG rootDocGia) {
  state.Reset();
  ThucHienTimKiemDocGia(rootDocGia, state);
}