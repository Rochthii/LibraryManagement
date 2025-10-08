
#ifndef DAUSACH_H
#define DAUSACH_H

#include <string>
#include <iostream>

const int MAX_SO_TRANG = 50000;
const int MAX_BAN_SAO = 10000;
const int MAX_DAUSACH = 1000;
const int MAX_KET_QUA_TIM_KIEM = 100;
const int MAX_TEN_SACH = 500;
const int MAX_TAC_GIA = 300;
const int MAX_THE_LOAI = 200;
const int MAX_VI_TRI_KE = 50;
const int MIN_TEN_SACH = 1;
const int MIN_TAC_GIA = 1;
const int MIN_THE_LOAI = 1;
const int NAM_XUAT_BAN_MIN = 1000;
const int NAM_XUAT_BAN_OFFSET_MAX = 5;
const int SO_LAN_THU_TOI_DA = 5;
const int SO_NGAY_MUON_TOI_DA = 7;

enum TrangThaiSach {
    CHO_MUON_DUOC = 0,  // Có sẵn để cho mượn
    DANG_MUON = 1,      // Đang được mượn
    THANH_LY = 2        // Đã thanh lý / loại bỏ
};

struct DanhMucSach {
    std::string maSach;         // Mã duy nhất cho từng cuốn
    TrangThaiSach trangThai;    // 0,1,2 theo enum
    std::string viTri;          // Vị trí trên kệ
    DanhMucSach* next = NULL; // Liên kết node kế tiếp
};

typedef DanhMucSach* PTRDMS;   

struct DauSach {
    std::string ISBN;
    std::string tenSach;
    int soTrang;
    std::string tacGia;
    int namXuatBan;
    std::string theLoai;
    PTRDMS dms = NULL;  
};

typedef DauSach* PTRDAUSACH;   

// bien toan cuc
extern PTRDAUSACH dsDauSach[MAX_DAUSACH];
extern int soLuongDauSach;

// Hàm cho Đầu Sách
bool themDauSach(PTRDAUSACH dsDauSach[], int& soLuongDauSach, const std::string& ISBN, const std::string& tenSach, int soTrang, const std::string& tacGia, int namXuatBan, const std::string& theLoai, bool anLang = false);
void inDanhSachTheoTheLoai(PTRDAUSACH dsDauSach[], int soLuongDauSach);
int timDauSach(PTRDAUSACH dsDauSach[], int soLuongDauSach, const std::string& tuKhoa);
void docDauSachTuFile(PTRDAUSACH dsDauSach[], int& soLuongDauSach);
void ghiDauSachVaoFile(PTRDAUSACH dsDauSach[], int soLuongDauSach);


// 
void sapXepDauSachTheoTen(PTRDAUSACH arr[], int low, int high);
int partition(PTRDAUSACH arr[], int low, int high);
void insertionSort(PTRDAUSACH arr[], int low, int high);

// ham cho danhmucsach
void themDanhMucSach(PTRDMS& dms, PTRDAUSACH dsDauSach[], int soLuongDauSach, const std::string& ISBN, int soLuong);
void capNhatTrangThaiSach(PTRDMS dms, std::string maSach, TrangThaiSach trangThai);

#endif
