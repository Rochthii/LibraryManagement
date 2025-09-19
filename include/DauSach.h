#ifndef DAUSACH_H
#define DAUSACH_H
#include<string>
#include<fstream>

struct DanhMucSach {
    std::string maSach;
    int TrangThai; // 0: cho muon dc, 1: da muon, 2: da thanh ly
    std::string viTri;
    DanhMucSach* next;
};

struct DauSach {
    std::string ISBN;
    std::string tenSach;
    int soTrang;
    std::string tacGia;
    int nanXuatBan;
    std::string theLoai;
    DanhMucSach* dms;
};

//bien toan cuc
extern DauSach** dsDauSach;
extern int soLuongDauSach;
extern int maxDauSach;

void themDauSach(std::string ISBN, std::string tenSach, int soTrang, std::string tacGia, int namXB, std::string theLoai); //c
DanhMucSach* themSach(std::string ISBN, int soLuongSach); //c
DauSach* timDauSach(std::string tenSach); //e
void inDanhSachDauSach(); //d
void saveDauSach(std::ofstream& file); // luu tru du lieu vao thuvien.dat
void loadDauSach(std::ifstream& file); //doc du lieu tu thuvien.dat
void capNhatTrangThaiSach(std::string MaSach, int TrangThai); // f va g

#endif