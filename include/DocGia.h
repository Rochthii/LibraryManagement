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
    NodeDG *left, *right;
};
typedef NodeDG* PTRDG;

PTRDG RotateLeft(PTRDG root);
PTRDG RotateRight(PTRDG root);
PTRDG InsertDocGia(PTRDG &pavltree, TheDocGia x);
//prototype

//quan ly cay doc gia
PTRDG themDocGia(PTRDG &root, PTRDG node);
void xoaDocGia(PTRDG &root, int mathe);
PTRDG timDocGia(PTRDG root, int mathe);
void inDocGiaInOrder(PTRDG root);
void hieuChinhDocGia(PTRDG root, int mathe);

//quan ly muon tra
void themMuonTra(PTRDG root, string maSach);
void saveDocGia(PTRDG root);
PTRDG loadDocGia();



#endif
