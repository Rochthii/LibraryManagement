#ifndef DAUSACH_H
#define DAUSACH_H

#include <string>

struct DanhMucSach;  // Forward declaration

const int MAX_DAUSACH = 1000;

struct DauSach {
    std::string ISBN;
    std::string tenSach;
    int soTrang;
    std::string tacGia;
    int namXuatBan;
    std::string theLoai;
    DanhMucSach* dms;
};

struct DanhMucSach {
    std::string maSach;
    int trangThai;  // 0: cho mượn được, 1: đang mượn, 2: thanh lý
    std::string viTri;
    DanhMucSach* next;
};

// Prototype hàm cho DauSach
void themDauSach(DauSach* dsDauSach[], int& soLuongDauSach);
void inDauSachTheoTheLoai(DauSach* dsDauSach[], int soLuongDauSach);
void timSachTheoTen(DauSach* dsDauSach[], int soLuongDauSach, std::string tenSach);
void docDauSachTuFile(DauSach* dsDauSach[], int& soLuongDauSach);
void ghiDauSachVaoFile(DauSach* dsDauSach[], int soLuongDauSach);

// Prototype hàm cho DanhMucSach
void themDanhMucSach(DanhMucSach*& dms, std::string ISBN, int soLuong);
void capNhatTrangThaiSach(DanhMucSach* dms, std::string maSach, int trangThai);

#endif
