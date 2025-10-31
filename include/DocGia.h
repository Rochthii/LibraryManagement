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
    
    // CẢI TIẾN: Thêm trường đếm sách để kiểm tra giới hạn O(1)
    int soSachDangMuon = 0; 
    
    MUONTRA dsmt;//danh sach muon tra linked list
};

//binary search tree (AVL)
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

// Prototype AVL
PTRDG RotateLeft(PTRDG root);
PTRDG RotateRight(PTRDG root);
void InsertDocGia(PTRDG& root, PTRDG node);

// Prototype Chức năng chính
void xoaDocGia(PTRDG& root, int mathe);
PTRDG timDocGia(PTRDG root, int mathe);
void inDocGiaInOrder(PTRDG root); // In theo Mã thẻ tăng dần
void display(PTRDG root); // Hàm in 1 độc giả

// CẢI TIẾN: Thay đổi prototype để tránh rò rỉ bộ nhớ
void hieuChinhDocGia(PTRDG root, const TheDocGia& new_data);

// Prototype cho chức năng sắp xếp (b)
void DuyetCayRaMang(PTRDG root, PTRDG arr[], int &count);
void InDocGiaTheoTenHo(PTRDG root); // In ra danh sách sắp xếp theo Tên+Họ
void QuickSortDocGia(PTRDG arr[], int low, int high);

// Prototype Giải phóng bộ nhớ
void GiaiPhongCay(PTRDG& root);
void giaiPhongDsmt(MUONTRA& dsmt);

// Prototype Quản lý Mượn Trả
void themMuonTra(PTRDG root, string maSach);
void themMuonTra(PTRDG docgia, const MuonTra &mt);

// Prototype File I/O
void saveDocGia(PTRDG root);
PTRDG loadDocGia();


#endif