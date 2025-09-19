#ifndef DAUSACH_H
#define DAUSACH_H

#include <string>

// Khai báo trước
struct DanhMucSach;

// Số lượng tối đa đầu sách
const int MAX_DAUSACH = 1000;

// Trạng thái sách trong danh mục
enum TrangThaiSach {
    CHO_MUON_DUOC = 0,  // Có sẵn để cho mượn
    DANG_MUON = 1,      // Đang được mượn
    THANH_LY = 2        // Đã thanh lý / loại bỏ
};

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
    int trangThai;     // 0,1,2 theo enum
    std::string viTri;
    DanhMucSach* next;
};

// Biến toàn cục
extern DauSach* dsDauSach[MAX_DAUSACH];
extern int soLuongDauSach;

// Hàm cho Đầu Sách
void themDauSach(DauSach* dsDauSach[], int& soLuongDauSach);
void inDauSachTheoTheLoai(DauSach* dsDauSach[], int soLuongDauSach);
void timSachTheoTen(DauSach* dsDauSach[], int soLuongDauSach, std::string tenSach);
void docDauSachTuFile(DauSach* dsDauSach[], int& soLuongDauSach);
void ghiDauSachVaoFile(DauSach* dsDauSach[], int soLuongDauSach);

// Hàm cho Danh Mục Sách
void themDanhMucSach(DanhMucSach*& dms, std::string ISBN, int soLuong);
void capNhatTrangThaiSach(DanhMucSach* dms, std::string maSach, int trangThai);

#endif
