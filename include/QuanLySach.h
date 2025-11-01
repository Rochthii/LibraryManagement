#ifndef QUANLYSACH_H
#define QUANLYSACH_H

#include <string>
#include <iostream>
#include "DauSach.h"

// bien toan cuc
extern PTRDS dsDauSach[MAX_DAUSACH];   // mang dau sach
extern int soLuongDauSach;             // so dau sach hien co
extern bool duLieuDaThayDoi;           // co thay doi du lieu

//  NHOM 1: SINH MA & TIEN ICH 
std::string sinhMaSach(const std::string &chiSo, int &soThuTu);                 // tao ma: ISBN-...
const char* TenTrangThai(TrangThaiSach tt);                                    // tra ve ten trang thai
int PhanTichTrangThaiSach(const std::string &s, std::ostream &out);            // chuoi -> so trang thai

//  NHOM 2: QUAN LY DANH MUC SACH (DMS - DSLK) 
std::string themDanhMucSach(PTRDMS &dms, const std::string &ma, TrangThaiSach tt, const std::string &vt);  // chen vao dau
bool ChenNodeDMSVaoDauSach(PTRDS d, const std::string &ma, int tt, const std::string &vt);               // chen vao dau dms
void capNhatTrangThaiSach(PTRDMS dms, const std::string &ma, TrangThaiSach tt);                         // cap nhat trang thai
PTRDMS timDanhMucTheoMaSach(const std::string &ma, PTRDS ds[], int n, std::ostream &out, bool silent = false); // tim node

//  NHOM 3: QUAN LY DAU SACH (DS - MANG CON TRO) 
bool themDauSach(PTRDS ds[], int &n, const std::string &isbn, const std::string &ten, int trang,
                 const std::string &tg, int nam, const std::string &tl, bool anLang = false);  // them + kiem tra
PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string &isbn);                           // tim theo isbn
void InMotDauSach(PTRDS d, std::ostream &out);                                                  // in chi tiet 1 dau sach

//  NHOM 4: LOGIC TIM KIEM & LIET KE 
struct KetQuaTimKiem {
    PTRDS sach;
    int loaiKhop;  // 1: ten, 2: tac gia, 3: the loai, 4: isbn
};

int timKiemLogic(PTRDS ds[], int n, const std::string &tk, KetQuaTimKiem kq[], int max);        // tim + sap xep
int TimTheLoaiDuyNhat(PTRDS ds[], int n, std::string tl[], int max);                           // danh sach the loai
int TimSachTheoTheLoai(PTRDS ds[], int n, const std::string &tl, PTRDS kq[], int max);         // sach theo the loai

//  NHOM 5: HAM CONSOLE (KHONG DUNG TRONG SFML) 
// void inDanhSachTheoTheLoai(PTRDS ds[], int n);     // chi dung console
// int timDauSach(PTRDS ds[], int n, const std::string &tk);  // chi dung console

//  NHOM 6: HAM SAP XEP 
void SapXepBanSaoTheoMa(PTRDMS arr[], int n);                                                  // bubble, chi swap sai thu tu
void sapXepDauSachTheoTen(PTRDS arr[], int low, int high);                                     // quick + insertion
void SapXepTheLoaiTheoTen(std::string arr[], int n);                                           // insertion sort
void hoanDoiDauSach(PTRDS &a, PTRDS &b);                                                       // hoan doi con tro
int partition(PTRDS arr[], int low, int high);                                                 // chia doi quicksort
void insertionSort(PTRDS arr[], int low, int high);                                            // insertion sort
void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n);                                          // bubble sort ket qua
void hoanDoiKetQua(KetQuaTimKiem &a, KetQuaTimKiem &b);                                       // hoan doi ket qua

//  NHOM 7: QUAN LY BO NHO 
void GiaiPhongDanhMucSach(PTRDMS &dms);                                                        // xoa dslk dms
void GiaiPhongDauSach(PTRDS &d);                                                               // xoa 1 dau sach
void GiaiPhongToanBoDauSach(PTRDS ds[], int &n);                                               // xoa toan bo
int DemTongSoBanSao(PTRDS ds[], int n);                                                        // dem tong ban sao
void CapNhatTongBanSao(PTRDS ds[], int n);                                                     // cap nhat field tongBanSao

//  NHOM 8: XOA & CAP NHAT 
std::string XoaDauSachTheoISBN(PTRDS ds[], int &n, const std::string &isbn);                   // xoa neu khong con dms
bool XoaSachTheoMaSach(PTRDS ds[], int n, const std::string &ma, std::ostream &out);           // xoa ban sao
std::string CapNhatDauSach(PTRDS ds[], int n, const std::string &isbn,
                           const std::string &ten, int trang, const std::string &tg,
                           int nam, const std::string &tl);                                    // cap nhat + sap xep
                                                        

#endif // QUANLYSACH_H