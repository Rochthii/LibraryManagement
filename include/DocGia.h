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
struct dsMuonTra{
    MuonTra data;
    dsMuonTra* next;
};
typedef dsMuonTra* MUONTRA;

//the doc gia
struct TheDocGia{
    int MaThe;
    string Ho;
    string Ten;
    bool Phai;//0 nam 1 nu
    int TrangThai;//0 = khoa, 1=hoat dong
    
    MUONTRA dsmt;//danh sach muon tra linked list
    
    TheDocGia* left;
    TheDocGia* right;
};
typedef TheDocGia* THE;

//binary search tree
struct BST{
    THE root;
};

//prototype

//quan ly cay doc gia
THE themDocGia(THE &root, THE node);
void xoaDocGia(THE &root, int mathe);
THE timDocGia(THE root, int mathe);
void inDocGiaInOrder(THE root);
void hieuChinhDocGia(THE root, int mathe);

//quan ly muon tra
void themMuonTra(THE root, string maSach);
void saveDocGia(THE root);
THE loadDocGia();



#endif
