#ifndef QUANLYSACH_H
#define QUANLYSACH_H

#include <string>
#include <iostream>
#include "DauSach.h"

// bien toan cuc
extern PTRDS dsDauSach[MAX_DAUSACH];   // mang dau sach
extern int soLuongDauSach;             // so dau sach hien co
extern bool duLieuDaThayDoi;           // co thay doi du lieu

//  NHOM 1: SINH MA & CHUYEN DOI TRANG THAI 
std::string sinhMaSach(const std::string &chiSo, int &soThuTu);                 // sinh ma: ISBN-1, ISBN-2,...
const char* TenTrangThai(TrangThaiSach tt);                                     // enum -> chuoi
int PhanTichTrangThaiSach(const std::string &s, std::ostream &out);             // chuoi -> enum

//  NHOM 2: QUAN LY DANH MUC SACH (DMS - DSLK DON) 
std::string themDanhMucSach(PTRDMS &dms, const std::string &ma, TrangThaiSach tt, const std::string &vt);  // chen vao dau
bool ChenNodeDMSVaoDauSach(PTRDS d, const std::string &ma, int tt, const std::string &vt);               // chen vao dau dms (dung khi doc file)
bool capNhatTrangThaiSach(PTRDMS dms, const std::string &ma, TrangThaiSach tt);                         // cap nhat trang thai ban sao
PTRDMS timDanhMucTheoMaSach(const std::string &ma, PTRDS ds[], int n, std::ostream &out, bool silent = false); // tim node theo ma sach

//  NHOM 3: QUAN LY DAU SACH (MANG CON TRO) 
bool themDauSach(PTRDS ds[], int &n, const std::string &isbn, const std::string &ten, int trang,
                 const std::string &tg, int nam, const std::string &tl, bool anLang = false);  // them + validation + sort
PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string &isbn);                           // linear search O(N)

//  NHOM 4: TIM KIEM & LIET KE 
struct KetQuaTimKiem {
    PTRDS sach;
    int loaiKhop;  // 1: ten sach, 2: tac gia, 3: the loai, 4: isbn
};

int timKiemLogic(PTRDS ds[], int n, const std::string &tk, KetQuaTimKiem kq[]);                 // tim + sap xep theo loai khop
int TimTheLoaiDuyNhat(PTRDS ds[], int n, std::string tl[]);                                     // lay danh sach the loai duy nhat
int TimSachTheoTheLoai(PTRDS ds[], int n, const std::string &tl, PTRDS kq[]);                   // loc sach theo 1 the loai

//  NHOM 5: SAP XEP 
void hoanDoiDauSach(PTRDS &a, PTRDS &b);                                                        // swap 2 con tro
void hoanDoiKetQua(KetQuaTimKiem &a, KetQuaTimKiem &b);                                         // swap 2 ket qua tim kiem
void insertionSort(PTRDS arr[], int left, int right);                                           // insertion sort cho mang nho
int partition(PTRDS arr[], int left, int right);                                                // partition cho quicksort
void sapXepDauSachTheoTen(PTRDS arr[], int left, int right);                                    // hybrid: quicksort + insertion
void SapXepBanSaoTheoMa(PTRDMS arr[], int n);                                                   // bubble sort ban sao theo so hau to
void SapXepTheLoaiTheoTen(std::string arr[], int n);                                            // insertion sort the loai
void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n);                                           // quicksort ket qua theo loai khop + ten

//  NHOM 6: QUAN LY BO NHO 
void GiaiPhongDanhMucSach(PTRDMS &dms);                                                         // xoa DSLK ban sao
void GiaiPhongDauSach(PTRDS &d);                                                                // xoa 1 dau sach + DSLK
void GiaiPhongToanBoDauSach(PTRDS ds[], int &n);                                                // xoa toan bo mang
int DemTongSoBanSao(PTRDS ds[], int n);                                                         // dem tong ban sao
void CapNhatTongBanSao(PTRDS ds[], int n);                                                      // cap nhat field tongBanSao

//  NHOM 7: XOA & CAP NHAT 
std::string XoaDauSachTheoISBN(PTRDS ds[], int &n, const std::string &isbn);                    // xoa dau sach (neu khong con ban sao)
bool XoaSachTheoMaSach(PTRDS ds[], int n, const std::string &ma, std::ostream &out);            // xoa 1 ban sao
std::string CapNhatDauSach(PTRDS ds[], int n, const std::string &isbn,
                           const std::string &ten, int trang, const std::string &tg,
                           int nam, const std::string &tl);                                     // cap nhat + sort neu ten doi

#endif // QUANLYSACH_H