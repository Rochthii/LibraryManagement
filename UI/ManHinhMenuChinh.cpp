#include "ManHinhMenuChinh.h"
#include "GiaoDienSFML.h"
#include "ManHinhQuanLyDocGia.h"
#include "ManHinhQuanLySach.h"
#include "TienIchGiaoDien.h"
#include "TrangThaiManHinhDocGia.h"
#include "TrangThaiManHinhMuonTra.h"

#include "GiaoDienMuonTra.h"

// Ve Man Hinh
void VeMenuChinhSFML(sf::RenderWindow &window, const sf::Font &font) {
  sf::Text tieuDe =
      TaoVanBan(font, "QUAN LY THU VIEN", FONT_SIZE_TIEU_DE_MAIN, MAU_TIEU_DE);
  CanGiuaGocXVanBan(tieuDe);
  tieuDe.setPosition(CHIEU_RONG / 2.0f, 130.f);
  window.draw(tieuDe);

  float nutRong = 350.f;
  float nutCao = 60.f;
  float khoangCachNut = 25.f;
  float x = (CHIEU_RONG - nutRong) / 2.0f;
  float y = 250.f;

  TaoNut(font, NUT_QUAN_LY_DAU_SACH, x, y, nutRong, nutCao,
         "1. QUAN LY DAU SACH", MAU_NEN_NUT, MAU_CHU_NUT);
  y += nutCao + khoangCachNut;
  TaoNut(font, NUT_MUON_TRA_SACH, x, y, nutRong, nutCao, "2. MUON TRA & DOC GIA",
         MAU_NEN_NUT, MAU_CHU_NUT);
  y += nutCao + khoangCachNut;
  TaoNut(font, NUT_THONG_TIN, x, y, nutRong, nutCao, "3. THONG TIN",
         MAU_NEN_NUT, MAU_CHU_NUT);
  y += nutCao + khoangCachNut;
  TaoNut(font, NUT_THOAT, x, y, nutRong, nutCao, "4. THOAT", MAU_NEN_NUT,
         MAU_CHU_NUT);
}

// Xu Ly Su Kien
void XuLySuKienMenuChinh(sf::RenderWindow &window, sf::Event event,
                         PTRDG rootDocGia, PTRDS dsDauSach[], int soLuongDauSach) {
  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      MaUI elementNhan =
          LayElementTaiToaDo(event.mouseButton.x, event.mouseButton.y);

      switch (elementNhan) {
      case NUT_QUAN_LY_DAU_SACH:
        manHinhHienTai = QUAN_LY_SACH;
        KhoiTaoManHinhSach(dsDauSach, soLuongDauSach); // Reset lai man hinh sach
        CapNhatThongBaoSFML("Chuyen den man hinh Quan Ly Sach.", 0);
        break;

      case NUT_MUON_TRA_SACH:
        manHinhHienTai = MUON_TRA_SACH;
        KhoiTaoManHinhMuonTra(rootDocGia); // Goi ham khoi tao man hinh Muon Tra
        CapNhatThongBaoSFML("Chuyen den man hinh Muon Tra Sach.", 0);
        break;

      case NUT_THONG_TIN:
        manHinhHienTai = MAN_HINH_THONG_TIN;
        CapNhatThongBaoSFML("Chuyen den man hinh Thong Tin.", 0);
        break;
      case NUT_THOAT:
        window.close();
        yeuCauThoat = true;
        break;

      default:
        break;
      }
    }
  }

  if (event.type == sf::Event::KeyPressed) {
    switch (event.key.code) {
    case sf::Keyboard::Num1:
    case sf::Keyboard::Numpad1:
      manHinhHienTai = QUAN_LY_SACH;
      KhoiTaoManHinhSach(dsDauSach, soLuongDauSach);
      CapNhatThongBaoSFML("Chuyen den man hinh Quan Ly Sach.", 0);
      break;

    case sf::Keyboard::Num2:
    case sf::Keyboard::Numpad2:
      manHinhHienTai = MUON_TRA_SACH;
      KhoiTaoManHinhMuonTra(rootDocGia);
      CapNhatThongBaoSFML("Chuyen den man hinh Muon Tra Sach.", 0);
      break;

    case sf::Keyboard::Num3:
    case sf::Keyboard::Numpad3:
      manHinhHienTai = MAN_HINH_THONG_TIN;
      CapNhatThongBaoSFML("Chuyen den man hinh Thong Tin.", 0);
      break;

    case sf::Keyboard::Num4:
    case sf::Keyboard::Numpad4:
      window.close();
      yeuCauThoat = true;
      break;

    default:
      break;
    }
  }
}