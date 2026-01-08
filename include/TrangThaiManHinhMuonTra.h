// [FILE: TrangThaiManHinhMuonTra.h - Phase 1 Update]
#ifndef TRANG_THAI_MAN_HINH_MUON_TRA_H
#define TRANG_THAI_MAN_HINH_MUON_TRA_H

#include "DocGia.h"
#include "MuonTra.h"
#include "QuanLySach.h"
#include <SFML/Graphics.hpp>
#include <string>


// =====// Enum che do hien thi man hinh chinh
enum CheDoHienThi {
  CHE_DO_MUON_TRA, // Muon/Tra sach
  CHE_DO_QUA_HAN   // Xem doc gia qua han
};

// Enum che do sap xep doc gia (tuong tu ManHinhQuanLyDocGia)
enum CheDoSapXepDocGia {
  SAP_XEP_THEO_MA_THE, // Sap xep theo ma the (InOrder)
  SAP_XEP_THEO_TEN_HO  // Sap xep theo ten+ho
};

// Hang so phan trang
const int DOC_GIA_MOI_TRANG = 14;
const int QUA_HAN_MOI_TRANG = 15;
const int SACH_MOI_TRANG_MUONTRA = 14;

// Use DTOs from Backend
typedef DocGiaTableDTO_Backend DocGiaTableDTO;
typedef SachTableDTO_Backend SachTableDTO;

// ===== STATE THONG NHAT =====
struct MuonTraState {
  // ----- CHE DO HIEN THI -----
  CheDoHienThi cheDoHienTai;
  CheDoSapXepDocGia cheDoSapXep;
  bool dangHienThiBangSach;
  bool dangHienThiFormThemDocGia;

  // ----- TRANG THAI BANG DOC GIA -----
  DocGiaTableDTO ketQuaTimKiemDocGia[MAX_DAUSACH];
  int soLuongKetQuaDocGia;
  int trangHienTaiDocGia;
  int tongSoTrangDocGia;

  // ----- TRANG THAI BANG SACH (SAU KHI CHON DOC GIA) -----
  SachTableDTO ketQuaTimKiemSach[MAX_DAUSACH];
  int soLuongKetQuaSach;
  int trangHienTaiSach;
  int tongSoTrangSach;

  // ----- FORM NHAP LIEU -----
  std::string chuoiTimKiemDocGia;
  std::string chuoiTimKiemSach;
  std::string chuoiMaSach;

  // ----- LOGIC STATE -----
  int maTheDocGiaDuocChon;
  PTRDG docGiaDangChon;
  std::string maSachDangChon;
  int indexSachDangChon;

  // ----- CACHE DATA -----
  ThongTinSachDangMuon_DTO listSachDangMuon[10];
  int slSachDangMuon;

  // ----- FORM THEM DOC GIA -----
  std::string chuoiHo;
  std::string chuoiTen;
  int phaiDuocChon;      // 0=Nam, 1=Nu
  int trangThaiDuocChon; // 0=Khoa, 1=Hoat dong
  bool dangSuaDocGia;

  // ----- DOUBLE CLICK -----
  sf::Clock doubleClickClock;
  int maTheClickCuoi;
  std::string isbnClickCuoi;
  const float THOI_GIAN_DOUBLE_CLICK = 0.3f;

  // ----- MODAL STATE -----
  bool hienThiModalTop10;
  bool hienThiModalQuaHan;
  bool hienThiModalChiTiet;
  TopSachDTO top10[10];
  int soLuongTop;
  DocGiaQuaHanDTO dsQuaHan[MAX_DAUSACH];
  int soLuongQuaHan;
  int trangQuaHanHienTai;
  int tongTrangQuaHan;

  // ----- XAC NHAN -----
  bool xacNhanMuon;
  bool xacNhanTra;
  bool xacNhanBaoMat;
  bool xacNhanXoaDocGia;

  // ===== CONSTRUCTOR =====
  MuonTraState() { Reset(); }

  void Reset() {
    cheDoHienTai = CHE_DO_MUON_TRA;
    cheDoSapXep = SAP_XEP_THEO_MA_THE;
    dangHienThiBangSach = false;
    dangHienThiFormThemDocGia = false;
    soLuongKetQuaDocGia = 0;
    trangHienTaiDocGia = 1;
    tongSoTrangDocGia = 1;
    soLuongKetQuaSach = 0;
    trangHienTaiSach = 1;
    tongSoTrangSach = 1;
    chuoiTimKiemDocGia = "";
    chuoiTimKiemSach = "";
    chuoiMaSach = "";
    maTheDocGiaDuocChon = 0;
    docGiaDangChon = nullptr;
    maSachDangChon = "";
    indexSachDangChon = -1;
    slSachDangMuon = 0;
    chuoiHo = "";
    chuoiTen = "";
    phaiDuocChon = 0;
    trangThaiDuocChon = 1;
    dangSuaDocGia = false;
    maTheClickCuoi = 0;
    isbnClickCuoi = "";
    hienThiModalTop10 = false;
    hienThiModalQuaHan = false;
    hienThiModalChiTiet = false;
    soLuongTop = 0;
    soLuongQuaHan = 0;
    trangQuaHanHienTai = 1;
    tongTrangQuaHan = 1;
    xacNhanMuon = false;
    xacNhanTra = false;
    xacNhanBaoMat = false;
    xacNhanXoaDocGia = false;
  }
};

void KhoiTaoManHinhMuonTra(PTRDG rootDocGia);
void VeManHinhMuonTra(sf::RenderWindow &window, const sf::Font &font);
void XuLySuKienManHinhMuonTra(sf::RenderWindow &window, sf::Event event, PTRDG rootDocGia);

#endif