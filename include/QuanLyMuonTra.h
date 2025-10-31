#ifndef QUAN_LY_MUON_TRA_H
#define QUAN_LY_MUON_TRA_H

#include <string>
#include "DocGia.h"
#include "NgayThang.h"

// ======================================================
// CAC HAM NGHIEP VU CHINH LIEN QUAN DEN MUON - TRA SACH
// ======================================================

// Kiem tra doc gia co sach qua han khong
// - Tra ve true neu co sach qua han > 7 ngay
bool kiemTraQuaHan(const MUONTRA* dsmt, const Date& ngayHienTai);

// Dem so luong sach doc gia dang muon (chua tra)
int demSachDangMuon(const MUONTRA* dsmt);

// ======================================================
// CAC HAM NGHIEP VU CHINH
// ======================================================

// Muon sach
// - Kiem tra ton tai doc gia va ma sach
// - Kiem tra trang thai the (chi cho muon neu trangThai = 1)
// - Kiem tra neu da muon >= 3 sach hoac co sach qua han thi khong duoc muon
// - Cap nhat danh sach muon tra va trang thai sach
// Tra ve: chuoi thong bao thanh cong hoac loi
std::string muonSach(PTRDG root, const std::string& maSach, int maThe, const Date& ngayMuon);

// Tra sach
// - Kiem tra ton tai doc gia va ma sach
// - Cap nhat ngay tra va trang thai
// - Neu lam mat sach thi trang thai = 2
std::string traSach(PTRDG root, const std::string& maSach, int maThe, const Date& ngayTra, bool matSach);

// Liet ke danh sach cac sach ma doc gia co so the X dang muon
// - Tra ve so luong sach
int lietKeSachDangMuon(PTRDG root, int maThe, MUONTRA arr[], int maxCount = 1000);

// Liet ke danh sach doc gia muon qua han
// - arr: mang ket qua
// - count: so luong ket qua tra ve
int lietKeDocGiaQuaHan(PTRDG root, int maThe[], int& count, const Date& ngayHienTai, int maxCount = 1000);

// Tinh so ngay qua han (giua ngayMuon va ngayHienTai)
int tinhSoNgayQuaHan(const Date& ngayMuon, const Date& ngayHienTai);

#endif // QUAN_LY_MUON_TRA_H
