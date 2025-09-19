#ifndef DOCGIA_H
#define DOCGIA_H
#include <string>
#include <ctime>
#include <fstream>

struct MuonTra {
    std::string MaSach;
    time_t NgayMuon;
    time_t NgayTra;  // 0 nếu chưa trả
    int TrangThai;    // 0: đang mượn, 1: đã trả, 2: mất sách
    MuonTra* next;
};

struct TheDocGia {
    int MaThe;        // Số ngẫu nhiên, duy nhất
    std::string Ho;
    std::string Ten;
    std::string Phai; // "Nam" hoặc "Nữ"
    int TrangThai;    // 0: khóa, 1: hoạt động
    MuonTra* dsMuonTra;
    TheDocGia* left;
    TheDocGia* right;
};

extern TheDocGia* rootDocGia;

TheDocGia* themDocGia(int MaThe, std::string Ho, std::string Ten, std::string Phai, int TrangThai);
void xoaDocGia(TheDocGia*& root, int MaThe);
TheDocGia* timDocGia(int MaThe);
void inDocGiaInorder(TheDocGia* root);
void themMuonTra(TheDocGia* docGia, std::string MaSach);
void saveDocGia(std::ofstream& file);
void loadDocGia(std::ifstream& file);

#endif