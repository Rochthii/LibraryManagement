#ifndef DOCGIA_H
#define DOCGIA_H

#include <string>
#include "DauSach.h"

struct MuonTra;  // Forward declaration

struct DocGia {
    int maThe;
    std::string ho;
    std::string ten;
    std::string phai;
    int trangThai;  // 0: khóa, 1: hoạt động
    MuonTra* dsMuonTra;
    DocGia* left;
    DocGia* right;
};

struct MuonTra {
    std::string maSach;
    std::string ngayMuon;
    std::string ngayTra;
    int trangThai;  // 0: đang mượn, 1: đã trả, 2: mất sách
    MuonTra* next;
};

// Prototype hàm cho DocGia
DocGia* timDocGia(int maThe);
void themDocGia(DocGia*& root);
void xoaDocGia(DocGia*& root, int maThe);
void hieuChinhDocGia(DocGia* root, int maThe);
void inDocGiaTheoTen(DocGia* root);
void inDocGiaTheoMa(DocGia* root);
void docDocGiaTuFile(DocGia*& root);
void ghiDocGiaVaoFile(DocGia* root);

// Prototype hàm cho MuonTra
void muonSach(DocGia* root, DauSach* dsDauSach[], int soLuongDauSach);
void traSach(DocGia* root, DauSach* dsDauSach[], int soLuongDauSach);
void lietKeSachDangMuon(DocGia* root, int maThe, DauSach* dsDauSach[], int soLuongDauSach);
void lietKeQuaHan(DocGia* root);
void top10SachMuonNhieu(DauSach* dsDauSach[], int soLuongDauSach, DocGia* root);

#endif
