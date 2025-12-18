#ifndef QUANLYSACH_H
#define QUANLYSACH_H

#include <string>
#include <iostream>
#include "DauSach.h"

// bien toan cuc
extern PTRDS dsDauSach[MAX_DAUSACH];   
extern int soLuongDauSach;             
extern bool duLieuDaThayDoi;           


// 1. Xu ly ma sach va trang thai

std::string sinhMaSach(const std::string &chiSo, int &soThuTu);                 
const char* TenTrangThai(TrangThaiSach tt);                                     
int PhanTichTrangThaiSach(const std::string &s, std::ostream &out);             


// 2. Quan ly Danh Muc Sach (DSLK)

// chen vao dau dslk O(1)
std::string themDanhMucSach(PTRDMS &dms, const std::string &ma, TrangThaiSach tt, const std::string &vt);  
bool ChenNodeDMSVaoDauSach(PTRDS d, const std::string &ma, int tt, const std::string &vt);               
bool capNhatTrangThaiSach(PTRDMS dms, const std::string &ma, TrangThaiSach tt);                         
PTRDMS timDanhMucTheoMaSach(const std::string &ma, PTRDS ds[], int n, std::ostream &out, bool silent = false); 
int TimSoThuTuLonNhat(PTRDS dauSach);                                                                     
std::string ThemNhieuBanSao(PTRDS dauSach, int soLuong, const std::string &viTri = "");                  


// 3. Quan ly Dau Sach (Mang Pointer)

bool themDauSach(PTRDS ds[], int &n, const std::string &isbn, const std::string &ten, int trang,
                 const std::string &tg, int nam, const std::string &tl, bool anLang = false);  
PTRDS TimDauSachTheoISBN(PTRDS ds[], int n, const std::string &isbn); // linear search O(N)                          


// 4. Tim kiem & Liet ke

struct KetQuaTimKiem {
    PTRDS sach;
    int loaiKhop;  
};

int timKiemLogic(PTRDS ds[], int n, const std::string &tk, KetQuaTimKiem kq[]); // linear search O(N)                
int TimTheLoaiDuyNhat(PTRDS ds[], int n, std::string tl[]);                                     
int TimViTriDuyNhat(PTRDS ds[], int n, std::string vt[]);                                       
int TimSachTheoTheLoai(PTRDS ds[], int n, const std::string &tl, PTRDS kq[]); // linear filter O(N)                 
int DemSoLuongBanSaoTheoTrangThai(PTRDS dauSach, TrangThaiSach trangThai);                     


struct ThongTinDauSach {
    int tongBanSao;
    int choMuonDuoc;
    int dangMuon;
    int thanhLy;
};

ThongTinDauSach LayThongTinDauSach(PTRDS dauSach);                                             


// 5. Sap xep

void hoanDoi(PTRDS &a, PTRDS &b);                                                        
void sapXepDauSachTheoTen(PTRDS arr[], int left, int right); // quicksort O(N log N)                                    
void SapXepBanSaoTheoMa(PTRDMS arr[], int n); // insertion sort O(N^2)                                                  
void SapXepTheLoaiTheoTen(std::string arr[], int n); // insertion sort O(N^2)                                            
void sapXepKetQuaTimKiem(KetQuaTimKiem arr[], int n); // insertion sort O(N^2)                                          


// 6. Quan ly bo nho

void GiaiPhongDanhMucSach(PTRDMS &dms);                                                         
void GiaiPhongDauSach(PTRDS &d);                                                                
void GiaiPhongToanBoDauSach(PTRDS ds[], int &n);                                                
int DemTongSoBanSao(PTRDS ds[], int n);                                                         
void CapNhatTongBanSao(PTRDS ds[], int n);                                                      


// 7. Xoa & Cap nhat

std::string XoaDauSach(PTRDS ds[], int &n, const std::string &isbn);                            
bool XoaSachTheoMaSach(PTRDS ds[], int n, const std::string &ma, std::ostream &out);            
std::string CapNhatDauSach(PTRDS ds[], int n, const std::string &isbn,
                           const std::string &ten, int trang, const std::string &tg,
                           int nam, const std::string &tl, const std::string &viTri = "");     
void CapNhatViTri(PTRDS ds[], int n, const std::string &isbn, const std::string &viTri);                  


// 8. Ham ho tro UI

void LayDanhSachTimKiem(const std::string& tuKhoa, KetQuaTimKiem* ketQua, int& soLuongKetQua); 
void LayDanhSachTheoTheLoai(std::string* cacTheLoai, int& soTheLoai);                          
void LayDanhSachViTri(std::string* cacViTri, int& soViTri);                                    
int LayDanhSachSachTheoTheLoai(const std::string& theLoai, PTRDS* ketQua, int maxKetQua);     
PTRDS TimDauSach(const std::string& isbn);                                                     
ThongTinDauSach LayThongTinDauSach(const std::string& isbn);                                   


// 9. Ham thao tac

std::string ThemDauSachMoi(const std::string& isbn, const std::string& ten, int trang,
                           const std::string& tg, int nam, const std::string& tl, 
                           int soLuongBanSao, const std::string& viTri = "");                  
std::string ThemBanSaoMoi(const std::string& isbn, int soLuong, const std::string& viTri = ""); 
std::string CapNhatThongTinDauSach(const std::string& isbn, const std::string& ten, int trang,
                                   const std::string& tg, int nam, const std::string& tl, 
                                   const std::string& viTri = "");                              
std::string XoaDauSachTheoISBN(const std::string& isbn);                                       
std::string ThanhLyBanSaoTheoMa(const std::string& maSach);                                    
std::string XoaBanSaoTheoMa(const std::string& maSach);                                        


// 10. Tien ich khac

void DanhDauDuLieuThayDoi();                                                                   
int LayDanhSachBanSaoSapXep(const std::string& isbn, PTRDMS* mangKetQua, int maxKetQua);      
std::string LayViTriDauTienCuaDauSach(const std::string& isbn);                               
bool CoTheBanSao(const std::string& isbn);                                                     

#endif // QUANLYSACH_H