#ifndef DOCGIA_H
#define DOCGIA_H
#include <string>
#include <fstream>
using namespace std;


//danh sach muon tra
struct MuonTra{
    string MaSach;
    string NgayMuon;
    string NgayTra;
    int TrangThai;//0= dang muon, 1 da tra, 2 la lam mat
};
struct NodeMT{
    MuonTra data;
    NodeMT* next;
};
typedef NodeMT* MUONTRA;

//PTRDGdoc gia
struct TheDocGia{
    int MaThe;
    string Ho;
    string Ten;
    bool Phai;//0 nam 1 nu
    int TrangThai = 1;//0 = khoa, 1=hoat dong
    
    MUONTRA dsmt;//danh sach muon tra linked list
};

//binary search tree
struct NodeDG{
    TheDocGia data;
    int bf;
    int height;
    NodeDG *left, *right;
};
typedef NodeDG* PTRDG;

//danh sach sach dang muon
struct ThongTinSachDangMuon {
    string MaSach;
    string NgayMuon;
    int SoNgayGiu;
    ThongTinSachDangMuon* next;
};
typedef ThongTinSachDangMuon* LIST_SACH_DANG_MUON;
//danh sach doc gia co sach qua han
struct DocGiaQuaHan {
    int MaThe;
    string HoTen;
    string MaSach;
    int SoNgayQuaHan;
    DocGiaQuaHan* next;
};
typedef DocGiaQuaHan* LIST_DOCGIA_QUAHAN;

PTRDG RotateLeft(PTRDG root);
PTRDG RotateRight(PTRDG root);
void InsertDocGia(PTRDG& root, PTRDG node);
//prototype

//quan ly cay doc gia
// PTRDG themDocGia removed - use InsertDocGia (AVL) instead
void xoaDocGia(PTRDG& root, int mathe);
PTRDG timDocGia(PTRDG root, int mathe);
void inDocGiaInOrder(PTRDG root);
void hieuChinhDocGia(PTRDG root, PTRDG pAlt, int mathe);

// them prototype giai phong cay
void GiaiPhongCay(PTRDG& root);

//quan ly muon tra
void themMuonTra(PTRDG root, string maSach);
void saveDocGia(PTRDG root);
PTRDG loadDocGia();



#endif
