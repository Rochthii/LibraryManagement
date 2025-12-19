#include "MuonTra.h"
#include "Constants.h"
#include "DauSach.h"
#include "DocGia.h"
#include "NgayThang.h"
#include "QuanLySach.h"
#include "XuLyChuoi.h"
#include <algorithm>
#include <iostream>


// --- SHARED DATA ---
extern PTRDG rootDocGia;
extern PTRDS dsDauSach[];
extern int soLuongDauSach;
extern bool duLieuDaThayDoi;

// --- HELPERS ---

static void QuickSortDocGiaLocal(PTRDG arr[], int left, int right) {
  int i = left, j = right;
  std::string pivot =
      arr[(left + right) / 2]->data.Ten + arr[(left + right) / 2]->data.Ho;
  while (i <= j) {
    while (arr[i]->data.Ten + arr[i]->data.Ho < pivot)
      i++;
    while (arr[j]->data.Ten + arr[j]->data.Ho > pivot)
      j--;
    if (i <= j) {
      std::swap(arr[i], arr[j]);
      i++;
      j--;
    }
  }
  if (left < j)
    QuickSortDocGiaLocal(arr, left, j);
  if (i < right)
    QuickSortDocGiaLocal(arr, i, right);
}

// --- DATA ACCESS & SEARCH ---

void LayDanhSachDocGiaBackend(const std::string &tuKhoa, bool laCheDoQuaHan,
                              bool sapXepTheoTen,
                              DocGiaTableDTO_Backend *ketQua, int &soLuong) {
  soLuong = 0;

  // Lay tat ca doc gia ra mang
  PTRDG mangTam[MAX_DAUSACH];
  int count = 0;
  DuyetCayRaMang(rootDocGia, mangTam, count);

  if (tuKhoa.empty()) {
    for (int i = 0; i < count && i < MAX_DAUSACH; ++i) {
      ketQua[soLuong].docGia = mangTam[i];
      ketQua[soLuong].loaiKhop = 2; // Show all
      ketQua[soLuong].overdueDays = 0;
      soLuong++;
    }
  } else {
    bool laTimTheoMa = true;
    for (char c : tuKhoa) {
      if (!isdigit(c)) {
        laTimTheoMa = false;
        break;
      }
    }

    if (laTimTheoMa) {
      int maThe = std::stoi(tuKhoa);
      for (int i = 0; i < count; ++i) {
        if (mangTam[i]->data.MaThe == maThe) {
          ketQua[soLuong].docGia = mangTam[i];
          ketQua[soLuong].loaiKhop = 0; // Exact
          ketQua[soLuong].overdueDays = 0;
          soLuong++;
          break;
        }
      }
    } else {
      std::string tuKhoaLower = ChuyenInThuong(tuKhoa);
      for (int i = 0; i < count && soLuong < MAX_DAUSACH; ++i) {
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
  }

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

    std::sort(
        ketQua, ketQua + soLuong,
        [](const DocGiaTableDTO_Backend &a, const DocGiaTableDTO_Backend &b) {
          return b.overdueDays < a.overdueDays;
        });
  } else if (sapXepTheoTen && soLuong > 0) {
    PTRDG mangSapXep[MAX_DAUSACH];
    for (int i = 0; i < soLuong; ++i)
      mangSapXep[i] = ketQua[i].docGia;
    QuickSortDocGiaLocal(mangSapXep, 0, soLuong - 1);
    for (int i = 0; i < soLuong; ++i)
      ketQua[i].docGia = mangSapXep[i];
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
  std::string loi = MuonSach(docGia, maSachChuan, dsDauSach, soLuongDauSach);
  if (loi.empty())
    duLieuDaThayDoi = true;
  return loi;
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
