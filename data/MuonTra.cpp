#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "Constants.h"
#include "DauSach.h"
#include "DocGia.h"
#include "KiemTraDuLieu.h"
#include "MuonTra.h"
#include "NgayThang.h"
#include "QuanLySach.h"
#include "TrangThaiManHinhMuonTra.h"
#include "XuLyChuoi.h"

// --- SHARED DATA ---
extern PTRDS dsDauSach[];
extern int soLuongDauSach;
extern bool duLieuDaThayDoi;

// --- HELPERS (Custom Algorithms) ---

static bool LaKyTuChuCaiASCII(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

/**
 * Strictly checks for unaccented ASCII letters and spaces.
 */
static std::string KiemTraKyTuTenASCII(const std::string &s,
                                       const std::string &fieldName) {
  for (size_t i = 0; i < s.length(); ++i) {
    char c = s[i];
    if (!LaKyTuChuCaiASCII(c) && c != ' ') {
      return "Loi: " + fieldName +
             " chi duoc chua chu cai khong dau (A-Z) va khoang trang!";
    }
  }
  return "";
}

std::string KiemTraDuLieuDocGia(const std::string &ho, const std::string &ten) {
  // 1. Clean & Standardize
  std::string hoChuan = ChuanHoaKhoangTrang(ho);
  std::string tenChuan = ChuanHoaKhoangTrang(ten);

  // 2. Check Empty & Length (using existing logic)
  std::string loiHo = KiemTraChuoiVaDodai(hoChuan, "Ho", 30);
  if (!loiHo.empty())
    return loiHo;

  std::string loiTen = KiemTraChuoiVaDodai(tenChuan, "Ten", 10);
  if (!loiTen.empty())
    return loiTen;

  // 3. Strict ASCII Check (No accents, symbols, or digits)
  std::string loiKyTuHo = KiemTraKyTuTenASCII(hoChuan, "Ho");
  if (!loiKyTuHo.empty())
    return loiKyTuHo;

  std::string loiKyTuTen = KiemTraKyTuTenASCII(tenChuan, "Ten");
  if (!loiKyTuTen.empty())
    return loiKyTuTen;

  return ""; // Valid
}

static void QuickSortByName(DocGiaTableDTO_Backend arr[], int left, int right) {
  int i = left, j = right;
  PTRDG pivotDg = arr[(left + right) / 2].docGia;
  std::string pivot = pivotDg->data.Ten + pivotDg->data.Ho;
  while (i <= j) {
    while (arr[i].docGia->data.Ten + arr[i].docGia->data.Ho < pivot)
      i++;
    while (arr[j].docGia->data.Ten + arr[j].docGia->data.Ho > pivot)
      j--;
    if (i <= j) {
      DocGiaTableDTO_Backend temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
      i++;
      j--;
    }
  }
  if (left < j)
    QuickSortByName(arr, left, j);
  if (i < right)
    QuickSortByName(arr, i, right);
}

static void QuickSortByOverdue(DocGiaTableDTO_Backend arr[], int left,
                               int right) {
  int i = left, j = right;
  int pivot = arr[(left + right) / 2].overdueDays;
  while (i <= j) {
    while (arr[i].overdueDays > pivot)
      i++; // Descending
    while (arr[j].overdueDays < pivot)
      j--;
    if (i <= j) {
      DocGiaTableDTO_Backend temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
      i++;
      j--;
    }
  }
  if (left < j)
    QuickSortByOverdue(arr, left, j);
  if (i < right)
    QuickSortByOverdue(arr, i, right);
}

static bool LaChuoiSo(const std::string &s) {
  if (s.empty())
    return false;
  for (size_t i = 0; i < s.length(); ++i) {
    if (s[i] < '0' || s[i] > '9')
      return false;
  }
  return true;
}

// --- DATA ACCESS & SEARCH ---

void LayDanhSachDocGiaBackend(PTRDG rootDocGia, std::string &tuKhoa, bool laCheDoQuaHan, bool sapXepTheoTen, DocGiaTableDTO_Backend *ketQua, int &soLuong) {
  soLuong = 0;

  // 1. TOI UU TIM KIEM THEO MA THE (O(log N))
  if (!tuKhoa.empty() && LaChuoiSo(tuKhoa) && !laCheDoQuaHan) {
    int maThe = 0;
    if (ChuyenChuoiThanhSoNguyen(tuKhoa, maThe, true)) {
      PTRDG dg = timDocGia(rootDocGia, maThe); // AVL Search O(log N)
      if (dg) {
        ketQua[soLuong].docGia = dg;
        ketQua[soLuong].loaiKhop = 0; // Exact
        ketQua[soLuong].overdueDays = 0;
        soLuong++;
      }
      return;
    }
  }

  // 2. TIM KIEM THEO TEN HOAC TAT CA (O(N))
  // Lay tat ca doc gia ra mang
  PTRDG mangTam[MAX_DAUSACH];
  int countTotal = 0;
  DuyetCayRaMang(rootDocGia, mangTam, countTotal);

  if (tuKhoa.empty()) {
    // Show all
    for (int i = 0; i < countTotal && i < MAX_DAUSACH; ++i) {
      ketQua[soLuong].docGia = mangTam[i];
      ketQua[soLuong].loaiKhop = 2;
      ketQua[soLuong].overdueDays = 0;
      soLuong++;
    }
  } else {
    // Partial Match (Keyword search)
    std::string tuKhoaLower = ChuyenInThuong(tuKhoa);
    for (int i = 0; i < countTotal && soLuong < MAX_DAUSACH; ++i) {
      std::string hoTenLower =
          ChuyenInThuong(mangTam[i]->data.Ho + " " + mangTam[i]->data.Ten);
      if (hoTenLower.find(tuKhoaLower) != std::string::npos) {
        ketQua[soLuong].docGia = mangTam[i];
        ketQua[soLuong].loaiKhop = 1; // Partial
        ketQua[soLuong].overdueDays = 0;
        soLuong++;
      }
    }
  }

  // 3. LOC QUA HAN
  if (laCheDoQuaHan) {
    DocGiaTableDTO_Backend ketQuaTam[MAX_DAUSACH];
    int soLuongTam = 0;
    for (int i = 0; i < soLuong; ++i) {
      PTRDG dg = ketQua[i].docGia;
      if (dg && dg->data.soSachDangMuon > 0) {
        int soNgayMax = TinhSoNgayQuaHanLonNhat(dg);
        if (soNgayMax > 7) {
          ketQuaTam[soLuongTam] = ketQua[i];
          ketQuaTam[soLuongTam].overdueDays = soNgayMax;
          soLuongTam++;
        }
      }
    }
    for (int i = 0; i < soLuongTam; ++i)
      ketQua[i] = ketQuaTam[i];
    soLuong = soLuongTam;

    // Custom Sort by Overdue Desc
    if (soLuong > 1) {
      QuickSortByOverdue(ketQua, 0, soLuong - 1);
    }
  } else if (sapXepTheoTen && soLuong > 1) {
    // Custom Sort by Name Asc
    QuickSortByName(ketQua, 0, soLuong - 1);
  }
}

void LayDanhSachSachBackend(const std::string &tuKhoa,
                            SachTableDTO_Backend *ketQua, int &soLuong) {
  soLuong = 0;
  if (tuKhoa.empty()) {
    for (int i = 0; i < soLuongDauSach && i < MAX_DAUSACH; ++i) {
      if (dsDauSach[i]) {
        ketQua[soLuong].dauSach = dsDauSach[i];
        ketQua[soLuong].loaiKhop = 2; // Show all
        soLuong++;
      }
    }
  } else {
    std::string tuKhoaLower = ChuyenInThuong(tuKhoa);
    for (int i = 0; i < soLuongDauSach && soLuong < MAX_DAUSACH; ++i) {
      if (!dsDauSach[i])
        continue;
      PTRDS dau = dsDauSach[i];
      std::string isbnLower = ChuyenInThuong(dau->ISBN);
      std::string tenLower = ChuyenInThuong(dau->tenSach);
      std::string tacGiaLower = ChuyenInThuong(dau->tacGia);

      bool khop = false;
      int loaiKhop = 1;

      if (isbnLower == tuKhoaLower) {
        khop = true;
        loaiKhop = 0;
      } else if (isbnLower.find(tuKhoaLower) != std::string::npos ||
                 tenLower.find(tuKhoaLower) != std::string::npos ||
                 tacGiaLower.find(tuKhoaLower) != std::string::npos) {
        khop = true;
      }

      if (khop) {
        ketQua[soLuong].dauSach = dau;
        ketQua[soLuong].loaiKhop = loaiKhop;
        soLuong++;
      }
    }
  }
}

int LayDSSachDangMuonBackend(PTRDG docGia, ThongTinSachDangMuon_DTO *ketQua) {
  if (!docGia)
    return 0;
  return LayDSSachDangMuon(docGia, ketQua, 10, dsDauSach, soLuongDauSach);
}

// --- TRANSACTIONS ---

std::string ThucHienMuonSachBackend(PTRDG docGia, const std::string &maSach) {
  if (!docGia)
    return "Loi: Chua chon doc gia!";
  std::string maSachChuan = CatKhoangTrang(maSach);
  if (maSachChuan.empty())
    return "Loi: Ma sach trong!";
  std::string result = MuonSach(docGia, maSachChuan, dsDauSach, soLuongDauSach);
  if (result.find("Loi:") == std::string::npos)
    duLieuDaThayDoi = true;
  return result;
}

std::string ThucHienTraSachBackend(PTRDG docGia, const std::string &maSach) {
  if (!docGia)
    return "Loi: Chua chon doc gia!";
  std::string loi = TraSach(docGia, maSach, dsDauSach, soLuongDauSach);
  if (loi.empty())
    duLieuDaThayDoi = true;
  return loi;
}

std::string ThucHienMatSachBackend(PTRDG docGia, const std::string &maSach) {
  if (!docGia)
    return "Loi: Chua chon doc gia!";
  std::string loi = BaoMatSach(docGia, maSach, dsDauSach, soLuongDauSach);
  if (loi.empty())
    duLieuDaThayDoi = true;
  return loi;
}

// --- UTILITIES ---

std::string TimMaSachCoTheMuon(const std::string &isbn) {
  PTRDMS mangBS[100];
  int nBS = LayDanhSachBanSaoSapXep(isbn, mangBS, 100);
  for (int i = 0; i < nBS; ++i) {
    if (mangBS[i]->trangThai == CHO_MUON_DUOC)
      return mangBS[i]->maSach;
  }
  return "";
}
