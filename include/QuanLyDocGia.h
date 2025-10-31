#ifndef QUAN_LY_DOC_GIA_H
#define QUAN_LY_DOC_GIA_H

#include <string>
#include "DocGia.h"

// ======================================================
// CAC HAM NGHIEP VU QUAN LY DOC GIA
// ======================================================

 //Them doc gia moi
 //- Kiem tra du lieu dau vao
 //- Tao doc gia moi va chen vao cay
 //- Luu lai file sau khi them
 //Tra ve: Chuoi thong bao thanh cong hoac loi
std::string themDocGia(PTRDG& root, const std::string& ho, const std::string& ten, bool phai, int trangThai);

// Xoa doc gia theo ma
// - Kiem tra ton tai
// - Kiem tra neu con sach dang muon thi khong duoc xoa
// - Luu file sau khi xoa
// Tra ve: Chuoi thong bao ket qua
std::string xoaDocGiaTheoMa(PTRDG& root, int maThe);

// Cap nhat thong tin doc gia
// - Kiem tra ton tai
// - Cap nhat cac truong duoc chinh sua
// - Luu file sau khi cap nhat
std::string capNhatDocGia(PTRDG root, int maThe,
    const std::string& ho, const std::string& ten,
    bool phai, int trangThai);

// ======================================================
// DU LIEU DON GIAN PHUC VU GIAO DIEN
// ======================================================

// Cau truc don gian de truyen du lieu len UI
struct DocGiaDonGian {
    int MaThe;
    std::string Ho;
    std::string Ten;
    bool Phai;
    int TrangThai;
};

// Lay danh sach tat ca doc gia (duyet theo thu tu ma the tang dan)
// - arr: mang luu ket qua
// - count: so luong doc gia tra ve
// - maxCount: gioi han toi da (mac dinh 1000)
void LayDanhSachDocGia(PTRDG root, DocGiaDonGian arr[], int& count, int maxCount = 1000);

#endif // QUAN_LY_DOC_GIA_H
