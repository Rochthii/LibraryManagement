#ifndef QUANLYSACH_H
#define QUANLYSACH_H

#include <string>
#include <iostream>
#include "DauSach.h"

// Hàm sinh ISBN và mã sách
std::string SinhISBN13TuDong(std::ostream& out);
std::string sinhMaSach(const std::string& chiSo, int& soThuTu, std::ostream& out);
const char* TenTrangThai(TrangThaiSach trangThai);

// Hàm quản lý danh mục sách
bool themDanhMucSach(PTRDMS& dms, const std::string& maSach, TrangThaiSach trangThai, const std::string& viTri);
void capNhatTrangThaiSach(PTRDMS dms, const std::string& maSach, TrangThaiSach trangThai);
PTRDMS timDanhMucTheoMaSach(const std::string& maSach, PTRDAUSACH ds[], int n, std::ostream& out, bool silent = true);
int PhanTichTrangThaiSach(const std::string& trangThai, std::ostream& out);

// Hàm quản lý đầu sách
bool themDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach, const std::string& ISBN, const std::string& tenSach, int soTrang, const std::string& tacGia, int namXuatBan, const std::string& theLoai, bool anLang);
void inDanhSachTheoTheLoai(PTRDAUSACH dsDauSach[], int soLuongDauSach);
int timDauSach(PTRDAUSACH dsDauSach[], int soLuongDauSach, const std::string& tuKhoa);
PTRDAUSACH TimDauSachTheoISBN(PTRDAUSACH ds[], int n, const std::string& isbn);
void InMotDauSach(PTRDAUSACH d, std::ostream& out);

// Hàm sắp xếp
void sapXepDauSachTheoTen(PTRDAUSACH arr[], int low, int high);
int partition(PTRDAUSACH arr[], int low, int high);
void insertionSort(PTRDAUSACH arr[], int low, int high);

// Hàm giải phóng bộ nhớ
void GiaiPhongDanhMucSach(PTRDMS& dms);
void GiaiPhongDauSach(PTRDAUSACH& dauSach);
void GiaiPhongToanBoDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach);
int DemTongSoBanSao(PTRDAUSACH dsDauSach[], int soLuongDauSach);
void CapNhatTongBanSao(PTRDAUSACH dsDauSach[], int soLuongDauSach);

#endif