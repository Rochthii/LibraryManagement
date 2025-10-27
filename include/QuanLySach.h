#ifndef QUANLYSACH_H
#define QUANLYSACH_H

#include <string>
#include <iostream>
#include "DauSach.h"


// Nhom 1: Sinh Ma & Tien Ich

std::string SinhISBN13TuDong(std::ostream& out);
std::string sinhMaSach(const std::string& chiSo, int& soThuTu, std::ostream& out);
const char* TenTrangThai(TrangThaiSach trangThai);
int PhanTichTrangThaiSach(const std::string& trangThai, std::ostream& out);


// Nhom 2: Quan Ly Danh Muc Sach (DMS - DSLK)

std::string themDanhMucSach(PTRDMS &dms, const std::string &maSach, TrangThaiSach trangThai, const std::string &viTri);
void capNhatTrangThaiSach(PTRDMS dms, const std::string& maSach, TrangThaiSach trangThai);
PTRDMS timDanhMucTheoMaSach(const std::string& maSach, PTRDS ds[], int n, std::ostream& out, bool silent = true);


// Nhom 3: Quan Ly Dau Sach (DS - Mang Con Tro)

bool themDauSach(PTRDS dsDauSach[], int& soLuongDauSach, const std::string& ISBN, const std::string& tenSach, int soTrang, const std::string& tacGia, int namXuatBan, const std::string& theLoai, bool anLang);
PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string& isbn);
void InMotDauSach(PTRDS d, std::ostream& out);


// Nhom 4: Logic Tim Kiem & Liet Ke (Tach biet khoi UI/Console)

struct KetQuaTimKiem {
    PTRDS sach;
    int loaiKhop; // 1: Ten, 2: Tac gia, 3: The loai, 4: ISBN
};

// Ham nay chi TIM, SAP XEP va tra ve ket qua (KHONG IN)
int timKiemLogic(PTRDS dsDauSach[], int soLuongDauSach, const std::string& tuKhoa, 
                 KetQuaTimKiem ketQua[], int maxKetQua);

// Ham 1: Lay danh sach cac the loai duy nhat
int TimTheLoaiDuyNhat(PTRDS dsDauSach[], int soLuongDauSach, 
                      std::string mangTheLoai[], int maxTheLoai);

// Ham 2: Lay danh sach sach theo mot the loai cu the
int TimSachTheoTheLoai(PTRDS dsDauSach[], int soLuongDauSach, 
                       const std::string& theLoai, PTRDS ketQua[], int maxSach);


// Nhom 5: Ham Console (Da Refactor)

void inDanhSachTheoTheLoai(PTRDS dsDauSach[], int soLuongDauSach);  //khong dung
int timDauSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string& tuKhoa); ///khong dung


// Nhom 6: Ham Sap Xep

// Sap xep Dau Sach (QuickSort + InsertionSort)
void sapXepDauSachTheoTen(PTRDS arr[], int low, int high);
void SapXepTheLoaiTheoTen(std::string arr[], int n);
int partition(PTRDS arr[], int low, int high);
void insertionSort(PTRDS arr[], int low, int high);
void hoanDoiDauSach(PTRDS& a, PTRDS& b);

// Sap xep Ket Qua Tim Kiem (Bubble Sort)
void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n);
void hoanDoiKetQua(KetQuaTimKiem& a, KetQuaTimKiem& b);


// Nhom 7: Quan Ly Bo Nho

void GiaiPhongDanhMucSach(PTRDMS& dms);
void GiaiPhongDauSach(PTRDS& dauSach);
void GiaiPhongToanBoDauSach(PTRDS dsDauSach[], int& soLuongDauSach);
int DemTongSoBanSao(PTRDS dsDauSach[], int soLuongDauSach);
void CapNhatTongBanSao(PTRDS dsDauSach[], int soLuongDauSach);

//Nhom 8: xoa dau sach
std::string XoaDauSachTheoISBN(PTRDS dsDauSach[], int& soLuongDauSach, const std::string& isbn);
bool XoaSachTheoMaSach(PTRDS dsDauSach[], int soLuongDauSach, const std::string& maSach, std::ostream& out);
std::string CapNhatDauSach(
    PTRDS dsDauSach[], int soLuongDauSach, const std::string& isbn,
    const std::string& tenSachMoi, int soTrangMoi, const std::string& tacGiaMoi,
    int namXuatBanMoi, const std::string& theLoaiMoi
);
#endif