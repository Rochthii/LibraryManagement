// [FILE: TrangThaiManHinhMuonTra.h - Phase 1 Update]
#ifndef TRANG_THAI_MAN_HINH_MUON_TRA_H
#define TRANG_THAI_MAN_HINH_MUON_TRA_H

#include "DocGia.h"
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
const int DOC_GIA_MOI_TRANG = 14; // Giam xuong 12 de tranh tran chu
const int QUA_HAN_MOI_TRANG = 15;
const int SACH_MOI_TRANG_MUONTRA = 14; // So sach hien thi khi da chon doc gia

// DTO cho doc gia hien thi trong bang
struct DocGiaTableDTO {
  PTRDG docGia;
  int loaiKhop; // 0: Exact, 1: Partial, 2: Show all
};

// DTO cho sach hien thi sau khi chon doc gia
struct SachTableDTO {
  PTRDS dauSach;
  int loaiKhop;
};

// ===== STATE THONG NHAT =====
struct MuonTraState {
  // ----- CHE DO HIEN THI -----
  CheDoHienThi cheDoHienTai;
  CheDoSapXepDocGia cheDoSapXep; // Che do sap xep doc gia
  bool dangHienThiBangSach; // true = hien bang sach, false = hien bang doc gia
  bool dangHienThiFormThemDocGia; // true = hien form them doc gia, false = hien
                                  // form muon/tra

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
  std::string chuoiTimKiemSach; // Tim sach khi da chon doc gia
  std::string chuoiMaSach;      // Ma sach can muon/tra

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
  bool hienThiModalChiTiet; // NEW: Toggle modal chi tiet doc gia
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
    // Che do mac dinh
    cheDoHienTai = CHE_DO_MUON_TRA;
    cheDoSapXep = SAP_XEP_THEO_MA_THE;
    dangHienThiBangSach = false;
    dangHienThiFormThemDocGia = false;

    // Bang doc gia
    soLuongKetQuaDocGia = 0;
    trangHienTaiDocGia = 1;
    tongSoTrangDocGia = 1;

    // Bang sach
    soLuongKetQuaSach = 0;
    trangHienTaiSach = 1;
    tongSoTrangSach = 1;
    dangHienThiBangSach = false;

    // Form
    chuoiTimKiemDocGia = "";
    chuoiTimKiemSach = "";
    chuoiMaSach = "";

    // Logic
    maTheDocGiaDuocChon = 0;
    docGiaDangChon = nullptr;
    maSachDangChon = "";
    indexSachDangChon = -1;

    // Cache
    slSachDangMuon = 0;

    // Form them doc gia
    chuoiHo = "";
    chuoiTen = "";
    phaiDuocChon = 0;
    trangThaiDuocChon = 1;
    dangSuaDocGia = false;

    // Double click
    maTheClickCuoi = 0;
    isbnClickCuoi = "";

    // Modal
    hienThiModalTop10 = false;
    hienThiModalQuaHan = false;
    hienThiModalChiTiet = false; // NEW
    soLuongTop = 0;
    soLuongQuaHan = 0;
    trangQuaHanHienTai = 1;
    tongTrangQuaHan = 1;

    // Xac nhan
    xacNhanMuon = false;
    xacNhanTra = false;
    xacNhanBaoMat = false;
    xacNhanXoaDocGia = false;
  }
};

// API
void KhoiTaoManHinhMuonTra();
void VeManHinhMuonTra(sf::RenderWindow &window, const sf::Font &font);
void XuLySuKienManHinhMuonTra(sf::RenderWindow &window, sf::Event event);

// ===== CAC ENUM CHO NUT GIAO DIEN (Them vao GiaoDienSFML.h) =====
// Them vao enum MaUI trong GiaoDienSFML.h:
// NUT_TAB_MUON_TRA,      // Tab chuyen che do Muon/Tra
// NUT_TAB_THEM_DOC_GIA,  // Tab chuyen che do Them Doc Gia
// NUT_TAB_QUA_HAN,       // Tab chuyen che do Qua Han
// NUT_QUAY_LAI_DS_DOC_GIA, // Nut quay lai bang doc gia tu bang sach
// INPUT_TIM_KIEM_SACH,   // O tim sach (khi da chon doc gia)

#endif